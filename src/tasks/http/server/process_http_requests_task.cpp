//
// Created by Chris Luttio on 3/27/21.
//

#include "process_http_requests_task.h"
#include "general/json.hpp"
#include "models/email_model.h"
#include "models/account_model.h"
#include "models/receive_rule.h"
#include "models/label_model.h"
#include "models/receive_rule_labels.h"

#include <algorithm>
#include <map>

using json = nlohmann::json;

void ProcessHTTPRequestsTask::perform() {
    for (const auto& envelope: state->inbound_http_request_queue) {
        auto request = envelope.request;
        auto client_request = std::make_shared<ClientRequest>();
        client_request->connection = envelope.connection;

        auto connection_header = request->headers.find("Connection");
        if (connection_header != request->headers.end()) {
            std::string value(*connection_header->second);
            std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::tolower(c); });
            if (value == "close") {
                envelope.connection->persistence = Connection::CLOSE;
            }
        }

        auto url = request->url.to_string();

        if (request->method == "GET") {
            bool websocket = false;
            if (url == "/") {
                if (request->headers.find("Connection") != request->headers.end() && request->headers.find("Upgrade") != request->headers.end()) {
                    if (*request->headers["Upgrade"] == "websocket") {
                        auto key = request->headers["Sec-WebSocket-Key"];
                        auto version = request->headers["Sec-WebSocket-Version"];
                        if (key != nullptr && version != nullptr) {
                            std::string accept = *key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

                            uint8_t buffer[20];
                            SHA1(reinterpret_cast<const uint8_t *>(accept.c_str()), accept.size(), buffer);
                            std::string encoded = encode_base_64(buffer, 20);

                            envelope.connection->protocol = WebSocket;
                            envelope.connection->websocket_state = CONNECTING;
                            client_request->type = WebSocketUpgrade;
                            client_request->response_headers["Sec-WebSocket-Accept"] = encoded;
                            client_request->status = Complete;
                            websocket = true;
                        }
                    }
                }
            } else if (url == "/emails") {
                client_request->type = ListMail;
                client_request->http_request = request;
            } else if (url.substr(0, 7) == "/emails" && url.size() > 8) {
                client_request->type = GetMail;
                client_request->uri = request->url;
                client_request->http_request = request;
            } else if (url == "/users") {
                client_request->type = BadRequest;
                client_request->status = Complete;
                try {
                    auto query = state->database->query("SELECT * FROM accounts");
                    std::vector<sql::account> accounts;
                    query.storein(accounts);
                    json output;
                    for (const auto& account : accounts) {
                        json content;
                        content["id"] = account.id;
                        content["username"] = account.username;
                        content["role"] = account.role;
                        output.push_back(content);
                    }
                    client_request->response_headers["Content-Type"] = "application/json";
                    client_request->data = std::make_shared<std::string>(output.dump());
                    client_request->type = OK;
                } catch (std::exception& e) {

                }
            } else if (url == "/labels") {
                client_request->type = BadRequest;
                client_request->status = Complete;
                try {
                    auto query = state->database->query("SELECT * FROM labels");
                    std::vector<sql::label> accounts;
                    query.storein(accounts);
                    json output;
                    for (const auto& account : accounts) {
                        json content;
                        content["id"] = account.id;
                        content["name"] = account.name;
                        output.push_back(content);
                    }
                    client_request->response_headers["Content-Type"] = "application/json";
                    client_request->data = std::make_shared<std::string>(output.dump());
                    client_request->type = OK;
                } catch (std::exception& e) {

                }
            } else if (url == "/rules") {
                client_request->type = BadRequest;
                client_request->status = Complete;
                try {
                    std::shared_ptr<UserAccount> user;
                    if (request->headers.find("Authorization") != request->headers.end()) {
                        auto header = request->headers["Authorization"];
                        if (header->size() > 7) {
                            auto token = header->substr(7);
                            user = state->get_user_from_token(token);
                        }
                    }
                    if (user != nullptr) {
                        auto query = state->database->query("SELECT * FROM rules WHERE account_id = %0 ORDER BY id");
                        query.parse();
                        auto rows = query.store(user->id);
                        if (!rows.empty()) {
                            json body;
                            json rule;
                            int id = 0;
                            for (const auto& row : rows) {
                                auto row_id = atoi(row["id"]);
                                if (id != row_id) {
                                    if (id != 0)
                                        body.push_back(rule);
                                    id = row_id;
                                    rule.clear();
                                    rule["id"] = row_id;
                                    rule["match"] = std::string(row["match"]);
                                    json account;
                                    account["id"] = atoi(row["account_id"]);
                                    account["username"] = std::string(row["account_username"]);
                                    rule["account"] = account;
                                }
                                json label;
                                label["id"] = atoi(row["label_id"]);
                                label["name"] = std::string(row["label_name"]);
                                rule["labels"].push_back(label);
                            }
                            body.push_back(rule);
                            client_request->data = std::make_shared<std::string>(body.dump());
                            client_request->type = OK;
                            client_request->response_headers["Content-Type"] = "application/json";
                        }
                    }
                } catch (std::exception& e) {

                }
            }
            if (!websocket && client_request->type == BadRequest) {
                if (state->routes.empty()) {
                    client_request->type = Requests::RetrieveFile;
                    client_request->uri = request->url;
                } else {
                    client_request->type = ResolveRoute;
                    client_request->uri = request->url;
                    client_request->http_request = request;
                }
            }
        }

        if (request->method == "POST") {
            if (url == "/authorize") {
                client_request->type = Authorize;
                client_request->data = request->body;
            } else if (url == "/authorize/refresh") {
                client_request->type = Refresh;
                client_request->data = request->body;
            } else if (url == "/track") {
                auto data = json::parse(*request->body);
                auto name = data["name"];
                auto session = std::make_shared<Session>(name, client_request->connection->socket.ip());
                state->sessions[session->id()] = session;
                client_request->data = std::make_shared<std::string>(json({{"id", session->id()}}).dump());
                client_request->type = Created;
                client_request->status = Complete;
            } else if (url.substr(0, 7) == "/track/") {
                auto session_id = url.substr(7);
                if (state->sessions.find(session_id) != state->sessions.end()) {
                    auto session = state->sessions[session_id];

                    json payload(*request->body);

                    SessionRecord record;
                    record.timestamp = std::chrono::high_resolution_clock::now();
                    if (payload.find("scroll_height") != payload.end())
                        record.scroll_height = payload["scroll_height"];
                    if (payload.find("window_height") != payload.end())
                        record.window_height = payload["window_height"];
                    if (payload.find("document_height") != payload.end())
                        record.document_height = payload["document_height"];

                    session->records.push_back(record);

                    client_request->type = Created;
                    client_request->status = Complete;
                }
            } else if (url == "/emails") {
                client_request->type = SendMail;
                client_request->data = request->body;
                client_request->http_request = request;
            } else if (url == "/users") {
                client_request->type = BadRequest;
                client_request->status = Complete;
                try {
                    json data = json::parse(*request->body);
                    auto query = state->database->query("INSERT INTO accounts (username, password, role) VALUES (%0, %1, %2)");
                    query.parse();
                    if (query.execute(to_string(data["username"]),
                                      to_string(data["password"]),
                                      to_string(data["role"]))) {
                        auto fetch = state->database->query("SELECT * FROM accounts WHERE username = %0");
                        fetch.parse();
                        sql::account user = fetch.store(to_string(data["username"]))[0];
                        auto account = std::make_shared<UserAccount>(data["username"], data["password"], data["role"]);
                        account->id = user.id;
                        state->accounts.push_back(account);
                        client_request->type = OK;
                    }
                } catch (std::exception& e) {

                }
            } else if (url.starts_with("/emails/") && url.ends_with("/labels")) {
                client_request->type = BadRequest;
                client_request->status = Complete;
                if (request->url.components.size() > 1) {
                    auto name = request->url.components[1];
                    try {
                        auto fetch_query = state->database->query("SELECT * FROM emails WHERE name = \"%0\"");
                        fetch_query.parse();
                        std::vector<sql::email> emails;
                        fetch_query.storein(emails, name);
                        bool ok = true;
                        if (emails.size() == 1) {
                            auto id = emails[0].id;
                            json body = json::parse(*request->body);
                            for (const auto& label : body) {
                                auto label_query = state->database->query("select * from labels where name = %0");
                                label_query.parse();
                                std::vector<sql::label> labels;
                                label_query.storein(labels, to_string(label));
                                if (!labels.empty()) {
                                    auto query = state->database->query("insert into email_labels (email_id, label_id) values (%0, %1)");
                                    query.parse();
                                    if (!query.execute(id, labels[0].id)) {
                                        ok = false;
                                    }
                                }
                            }
                        }
                        if (ok)
                            client_request->type = OK;
                    } catch (std::exception& e) {
                        std::cerr << e.what() << std::endl;
                    }
                }
            } else if (url == "/rules") {
                client_request->type = BadRequest;
                client_request->status = Complete;
                try {
                    std::shared_ptr<UserAccount> user;
                    if (request->headers.find("Authorization") != request->headers.end()) {
                        auto header = request->headers["Authorization"];
                        if (header->size() > 7) {
                            auto token = header->substr(7);
                            user = state->get_user_from_token(token);
                        }
                    }
                    if (user != nullptr) {
                        json data = json::parse(*request->body);
                        if (data.contains("match") && data.contains("labels")) {
                            if (data["match"].is_string() && data["labels"].is_array()) {
                                auto query = state->database->query("INSERT INTO receive_rules (`match`, account_id) VALUES (%0, %1)");
                                query.parse();
                                auto rule_id = query.execute(to_string(data["match"]), std::to_string(user->id)).insert_id();
                                if (rule_id > 0) {
                                    bool ok = true;
                                    for (const auto& label : data["labels"]) {
                                        if (label.is_string()) {
                                            auto fetch_label_query = state->database->query("SELECT * FROM labels WHERE name = %0");
                                            fetch_label_query.parse();
                                            std::vector<sql::label> labels;
                                            fetch_label_query.storein(labels, to_string(label));
                                            if (!labels.empty()) {
                                                auto create_rule_label_query = state->database->query("INSERT INTO receive_rule_labels (rule_id, label_id) values (%0, %1)");
                                                create_rule_label_query.parse();
                                                if (!create_rule_label_query.execute(rule_id, labels[0].id)) {
                                                    ok = false;
                                                }
                                            } else {
                                                ok = false;
                                            }
                                        }
                                    }
                                    if (ok)
                                        client_request->type = OK;
                                }
                            }
                        }
                    }
                } catch (std::exception& e) {

                }
            }
        }

        if (request->method == "PUT") {
            if (url.starts_with("/users/")) {
                client_request->type = BadRequest;
                client_request->status = Complete;
                if (request->url.components.size() > 1) {
                    auto id = request->url.components[1];
                    try {
                        json body = json::parse(*request->body);
                        auto fetch = state->database->query("SELECT * FROM accounts WHERE id = %0");
                        fetch.parse();
                        sql::account user = fetch.store(id)[0];
                        if (body.contains("username"))
                            user.username = body["username"];
                        if (body.contains("password"))
                            user.password = body["password"];
                        if (body.contains("role"))
                            user.role = body["role"];
                        auto query = state->database->query("UPDATE accounts SET username = %1q, password = %2q, role = %3q WHERE id = %0");
                        query.parse();
                        if (query.execute(id,
                                          user.username,
                                          user.password,
                                          user.role)) {
                            for (auto& a : state->accounts) {
                                if (a->id == user.id) {
                                    a->username = user.username;
                                    a->password = user.password;
                                    a->role = user.role;
                                    break;
                                }
                            }
                            client_request->type = OK;
                        }
                    } catch (std::exception& e) {
                        std::cerr << e.what() << std::endl;
                    }
                }
            }
        }

        if (request->method == "DELETE") {
            if (url.starts_with("/emails/")) {
                client_request->type = BadRequest;
                client_request->status = Complete;
                if (request->url.components.size() > 1) {
                    auto name = request->url.components[1];
                    try {
                        auto fetch_query = state->database->query("SELECT * FROM emails WHERE name = \"%0\"");
                        fetch_query.parse();
                        std::vector<sql::email> emails;
                        fetch_query.storein(emails, name);
                        if (emails.size() == 1) {
                            auto query = state->database->query("DELETE FROM emails WHERE id = %0q");
                            query.parse();
                            if (query.execute(emails[0].id)) {
                                client_request->type = OK;
                            }
                        }
                    } catch (std::exception& e) {
                        std::cerr << e.what() << std::endl;
                    }
                }
            } else if (url.starts_with("/users/")) {
                client_request->type = BadRequest;
                client_request->status = Complete;
                if (request->url.components.size() > 1) {
                    auto id = request->url.components[1];
                    try {
                        auto query = state->database->query("DELETE FROM accounts WHERE id = %0");
                        query.parse();
                        if (query.execute(id)) {
                            for (auto i = state->accounts.begin(); i != state->accounts.end(); i++) {
                                if ((*i)->id == atoi(id.c_str())) {
                                    state->accounts.erase(i);
                                    break;
                                }
                            }
                            client_request->type = OK;
                        }
                    } catch (std::exception& e) {
                        std::cerr << e.what() << std::endl;
                    }
                }
            } else if (url.starts_with("/rules/")) {
                client_request->type = BadRequest;
                client_request->status = Complete;
                try {
                    std::shared_ptr<UserAccount> user;
                    if (request->headers.find("Authorization") != request->headers.end()) {
                        auto header = request->headers["Authorization"];
                        if (header->size() > 7) {
                            auto token = header->substr(7);
                            user = state->get_user_from_token(token);
                        }
                    }
                    if (user != nullptr) {
                        if (request->url.components.size() > 1) {
                            auto id = request->url.components[1];
                            auto query = state->database->query("DELETE FROM receive_rules WHERE id = %0");
                            query.parse();
                            if (query.execute(id)) {
                                client_request->type = OK;
                            }
                        }
                    }
                } catch (std::exception& e) {

                }
            }
        }

        if (client_request->type == BadRequest) {
            client_request->connection->persistence = Connection::CLOSE;
            client_request->status = Complete;
        }

        _controller->apply(Action(CreateClientRequest, client_request));
    }
    _controller->apply(Action(ClearHttpRequests));
}
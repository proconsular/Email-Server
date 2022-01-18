//
// Created by Chris Luttio on 3/27/21.
//

#include "initialize_client_requests_task.h"
#include "load_requested_file_task.h"
#include "utils.h"
#include "route_resolver.h"
#include "tasks/authorize_user_task.h"
#include "jwt-cpp/jwt.h"
#include "json.hpp"
#include "objects/email_parser.h"

#include "models/email_model.h"

using json = nlohmann::json;

void InitializeClientRequestsTask::perform() {
    for (const auto& pair: state->requests) {
        auto request = std::make_shared<ClientRequest>(*pair.second);
        if (request->status == RequestStatus::New) {
            switch (request->type) {
                case RetrieveFile: {
                    state->scheduler->add(std::make_shared<LoadRequestedFileTask>(_controller, request, state->config));
                    request->status = RequestStatus::Working;
                    _controller->apply(Action(ModifyClientRequest, request));
                    break;
                }
                case ResolveRoute: {
                    auto headers = request->http_request->headers;

                    RouteResolver resolver;
                    resolver.resolve(state->routes, request->uri.to_string());

                    bool authorized = false;

                    if (resolver.attributes.find("Access") != resolver.attributes.end()) {
                        std::string role = resolver.attributes["Access"];
                        if (role != "public") {
                            if (headers.find("Authorization") != headers.end()) {
                                auto authorization = headers["Authorization"];
                                auto components = split_string(" ", *authorization);
                                if (components.size() == 2 && components[0] == "Bearer") {
                                    std::string access_token = components[1];

                                    for (const auto& account : state->accounts) {
                                        if (account->role == role) {
                                            for (const auto& auth : account->authorizations) {
                                                if (auth.access_token == access_token) {
                                                    auto decoded = jwt::decode(access_token);

                                                    auto verifier = jwt::verify()
                                                            .allow_algorithm(jwt::algorithm::hs256{"helloworld"})
                                                            .with_issuer("auth0");

                                                    authorized = true;

                                                    try {
                                                        verifier.verify(decoded);
                                                    } catch (std::exception& e) {
                                                        authorized = false;
                                                    }

                                                    break;
                                                }
                                            }
                                            if (authorized)
                                                break;
                                        }
                                    }
                                }
                            }
                        } else {
                            authorized = true;
                        }
                    } else {
                        authorized = true;
                    }

                    if (authorized) {
                        request->response_headers = resolver.attributes;
                        request->uri = URL::parse(resolver.url);
                        request->type = RetrieveFile;
                    } else {
                        request->type = Forbidden;
                        request->status = Failed;
                    }

                    _controller->apply(Action(ModifyClientRequest, request));
                    break;
                }
                case Authorize: {
                    state->scheduler->add(std::make_shared<AuthorizeUserTask>(state, _controller, request));
                    request->status = Working;
                    _controller->apply(Action(ModifyClientRequest, request));
                    break;
                }
                case Refresh: {

                    bool authorized = false;

                    std::string new_token;

                    if (!request->data->empty()) {
                        json body = json::parse(*request->data);
                        std::string refresh_token;
                        if (body.find("refresh_token") != body.end())
                            refresh_token = body["refresh_token"];

                        for (const auto& account: state->accounts) {
                            for (auto& auth: account->authorizations) {
                                if (auth.refresh_token == refresh_token) {
                                    auto decoded = jwt::decode(refresh_token);

                                    auto verifier = jwt::verify()
                                            .allow_algorithm(jwt::algorithm::hs256{"helloworld"})
                                            .with_issuer("auth0");

                                    authorized = true;

                                    try {
                                        verifier.verify(decoded);
                                    } catch (std::exception& e) {
                                        authorized = false;
                                    }

                                    if (authorized) {
                                        auto now = std::chrono::system_clock::now();
                                        auth.access_token = jwt::create()
                                                .set_issuer("auth0")
                                                .set_type("JWT")
                                                .set_issued_at(now)
                                                .set_expires_at(now + std::chrono::minutes{60})
                                                .set_payload_claim("username", jwt::claim(std::string(account->username)))
                                                .set_payload_claim("password", jwt::claim(std::string(account->password)))
                                                .set_payload_claim("client_id", jwt::claim(auth.client_id))
                                                .sign(jwt::algorithm::hs256{"helloworld"});
                                        new_token = auth.access_token;
                                    }

                                    break;
                                }
                            }
                            if (authorized)
                                break;
                        }
                    }

                    if (authorized) {
                        json body({{"access_token", new_token}});
                        request->data = std::make_shared<std::string>(body.dump());
                        request->status = Complete;
                    } else {
                        request->type = Authorize;
                        request->status = Failed;
                    }

                    _controller->apply(Action(ModifyClientRequest, request));
                    break;
                }
                case SendMail: {
                    std::shared_ptr<UserAccount> user;
                    if (authorize(request, user)) {
                        json body = json::parse(*request->data);
                        auto message = Email::create(body["from"], body["to"], body["subject"], std::make_shared<std::string>(body["body"]));
                        state->mail->append_send_queue(message);
                        request->status = Complete;
                    } else {
                        request->type = Forbidden;
                        request->status = Failed;
                    }
                    _controller->apply(Action(ModifyClientRequest, request));
                    break;
                }
                case ListMail: {
                    std::shared_ptr<UserAccount> user;
                    if (authorize(request, user)) {
                        auto sql =
                                "SELECT * "
                                "FROM emails_with_labels "
                                "WHERE account_id = %0 "
                                "ORDER BY id DESC";
                        auto list_query = state->database->query(sql);
                        list_query.parse();
                        auto rows = list_query.store(user->id);
                        int last_id = 0;
                        std::vector<json> labels;
                        json output;
                        for (int i = 0; i < rows.size(); i++) {
                            auto row = rows[i];
                            int id = atoi(row["id"]);
                            if (!row["label_id"].is_null()) {
                                json label;
                                label["id"] = atoi(row["label_id"]);
                                label["name"] = row["label_name"].c_str();
                                labels.push_back(label);
                            }
                            if ((i + 1 < rows.size() && atoi(rows[i + 1]["id"]) != id) || i + 1 == rows.size()) {
                                json summary;
                                summary["id"] = id;
                                summary["message-id"] = row["name"].c_str();
                                summary["sender"] = row["sender"].c_str();
                                summary["to"] = row["to"].c_str();
                                summary["subject"] = row["subject"].c_str();
                                summary["created_at"] = row["created_at"].c_str();
                                summary["account_id"] = atoi(row["account_id"]);
                                summary["labels"] = labels;
                                output.push_back(summary);
                                labels.clear();
                            }
                            last_id = id;
                        }
                        request->data = std::make_shared<std::string>(output.dump());
                        request->response_headers["Content-Type"] = "application/json";
                        request->type = OK;
                        request->status = Complete;
                    } else {
                        request->type = Forbidden;
                        request->status = Failed;
                    }
                    _controller->apply(Action(ModifyClientRequest, request));
                    break;
                }
                case GetMail: {
                    std::shared_ptr<UserAccount> user;
                    if (authorize(request, user)) {
                        request->type = NotFound;
                        request->status = Complete;
                        if (request->uri.components.size() == 2) {
                            auto endpoint = request->uri.components[1];
                            std::string name;
                            std::map<std::string, std::string> query;

                            int i = 0;
                            while (i < endpoint.size() && endpoint[i] != '?') i++;
                            name = endpoint.substr(0, i);
                            if (i < endpoint.size() && endpoint[i] == '?') {
                                i++;
                                while (i < endpoint.size()) {
                                    int s = i;
                                    while (i < endpoint.size() && endpoint[i] != '=') i++;
                                    std::string key = endpoint.substr(s, i - s);
                                    i++;
                                    s = i;
                                    std::string value;
                                    while (i < endpoint.size() && endpoint[i] != '&') {
                                        if (endpoint[i] == '_') {
                                            value += '/';
                                        } else {
                                            value += endpoint[i];
                                        }
                                        i++;
                                    }
                                    query[key] = value;
                                    i++;
                                }
                            }

                            auto path = "emails/" + name + ".eml";
                            FILE* file = fopen(path.c_str(), "r");
                            if (file) {
                                auto data = std::make_shared<std::string>();
                                const size_t len = 10 * 1024;
                                char *buffer = new char[len];
                                size_t amount = 0;
                                do {
                                    amount = fread(buffer, 1, len, file);
                                    data->append(std::string(buffer, buffer + amount));
                                } while (amount == len);
                                delete[] buffer;
                                fclose(file);

                                auto output = std::make_shared<std::string>();
                                std::string type = "text/plain";

                                try {
                                    auto email = EmailParser::parse(data);
                                    auto content_type = email->header->get_content_type_header();
                                    if (content_type == nullptr) {
                                        output->append(email->generate_body());
                                    } else {
                                        if (content_type->type != "multipart") {
                                            output->append(email->generate_body());
                                        } else {
                                            if (content_type->subtype == "alternative" && !query.empty()) {
                                                if (query.find("accept") != query.end())
                                                    type = query["accept"];
                                                const BodyPart* part = email->body->find_alternative(type);
                                                if (part == nullptr)
                                                    part = email->body->find_alternative("text/plain");
                                                if (part != nullptr && part->content) {
                                                    output->append(*part->content);
                                                } else {
                                                    output->append(email->generate_body());
                                                }
                                            } else {
                                                output->append(email->generate_body());
                                            }
                                        }
                                    }
                                } catch (std::exception& e) {
                                    std::cerr << e.what() << std::endl;
                                }

                                request->type = OK;
                                request->data = output;
                                request->response_headers["Content-Type"] = type;
                            }
                        }
                    } else {
                        request->type = Forbidden;
                        request->status = Failed;
                    }
                    _controller->apply(Action(ModifyClientRequest, request));
                    break;
                }
                default:
                    break;
            }
        }
    }
}

bool InitializeClientRequestsTask::authorize(const std::shared_ptr<ClientRequest> &request, std::shared_ptr<UserAccount> &user) {
    std::string token;
    auto headers = request->http_request->headers;
    if (headers.find("Authorization") != headers.end()) {
        auto auth = headers["Authorization"];
        if (auth->size() > 7)
            token = auth->substr(7);
    }
    return state->authenticator->authenicate(token, user);
}
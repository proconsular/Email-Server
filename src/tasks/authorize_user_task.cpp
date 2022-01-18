//
// Created by Chris Luttio on 5/8/21.
//

#include "authorize_user_task.h"

#include "json.hpp"
#include "jwt-cpp/jwt.h"
#include "models/account_model.h"

using json = nlohmann::json;

void AuthorizeUserTask::perform() {
    if (!_request->data->empty()) {
        try {
            json body = json::parse(*_request->data);
            std::string username;
            std::string password;
            if (body.contains("username"))
                username = body["username"];
            if (body.contains("password"))
                password = body["password"];
            std::shared_ptr<UserAccount> account;
            for (const auto& user : _state->accounts) {
                if (user->username == username) {
                    account = user;
                    break;
                }
            }
            if (account != nullptr) {
                if (password == account->password) {
                    auto now = std::chrono::system_clock::now();

                    Authorization authorization;
                    authorization.client_id = generate_hash_id(10);
                    authorization.created_at = now;
                    authorization.access_token = jwt::create()
                            .set_issuer("auth0")
                            .set_type("JWT")
                            .set_issued_at(now)
                            .set_expires_at(now + std::chrono::minutes{60})
                            .set_payload_claim("username", jwt::claim(std::string(account->username)))
                            .set_payload_claim("password", jwt::claim(std::string(account->password)))
                            .set_payload_claim("client_id", jwt::claim(authorization.client_id))
                            .sign(jwt::algorithm::hs256{"helloworld"});
                    authorization.refresh_token = jwt::create()
                            .set_issuer("auth0")
                            .set_type("JWT")
                            .set_issued_at(now)
                            .set_expires_at(now + std::chrono::hours{24 * 10})
                            .set_payload_claim("username", jwt::claim(std::string(account->username)))
                            .set_payload_claim("password", jwt::claim(std::string(account->password)))
                            .set_payload_claim("client_id", jwt::claim(authorization.client_id))
                            .sign(jwt::algorithm::hs256{"helloworld"});

                    authorization.ip = _request->connection->socket.ip();

                    account->authorizations.push_back(authorization);
                    json output({
                        {"client_id", authorization.client_id},
                        {"access_token", authorization.access_token},
                        {"refresh_token", authorization.refresh_token},
                    });

                    try {
                        auto query = _state->database->query("UPDATE accounts SET access_token = (%0q), refresh_token = (%1q) WHERE id = " + std::to_string(account->id));
                        query.parse();
                        query.execute(authorization.access_token, authorization.refresh_token);
                    } catch(const mysqlpp::Exception& e) {
                        std::cerr << "err: " << e.what() << std::endl;
                    }

                    _request->data = std::make_shared<std::string>(output.dump());
                    _request->status = Complete;
                } else {
                    _request->status = Failed;
                }
            } else {
                _request->status = Failed;
            }
        } catch (std::exception& e) {
            _request->status = Failed;
        }
    } else {
        _request->status = Failed;
    }
    _controller->apply(Action(ModifyClientRequest, _request));
    _alive = false;
}
//
// Created by Chris Luttio on 6/21/21.
//

#include "authenicator.h"
#include "jwt-cpp/jwt.h"
#include "state.h"

bool Authenticator::authenicate(const std::string &token, std::shared_ptr<UserAccount> &user) {
    for (const auto& account : _state->accounts) {
        for (const auto& auth : account->authorizations) {
            if (auth.access_token == token) {
                auto decoded = jwt::decode(token);

                auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{"helloworld"})
                        .with_issuer("auth0");

                try {
                    verifier.verify(decoded);
                } catch (std::exception& e) {
                    return false;
                }

                user = account;
                return true;
            }
        }
    }
    return false;
}
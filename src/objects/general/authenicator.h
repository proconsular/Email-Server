//
// Created by Chris Luttio on 6/21/21.
//

#ifndef P11_MAIL_SERVER_AUTHENICATOR_H
#define P11_MAIL_SERVER_AUTHENICATOR_H

#include <utility>
#include <memory>

#include "user_account.h"

class State;

class Authenticator {
public:
    explicit Authenticator(std::shared_ptr<State> state): _state(std::move(state)) {}

    bool authenicate(const std::string& token, std::shared_ptr<UserAccount>& user);
private:
    std::shared_ptr<State> _state;
};


#endif //P11_MAIL_SERVER_AUTHENICATOR_H

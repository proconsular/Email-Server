//
// Created by Chris Luttio on 5/8/21.
//

#ifndef P8_WEB_SERVER_USER_ACCOUNT_H
#define P8_WEB_SERVER_USER_ACCOUNT_H

#include <string>
#include <utility>
#include <vector>

#include "authorization.h"

class UserAccount {
public:
    explicit UserAccount() = default;
    UserAccount(std::string username, std::string password, std::string role): username(std::move(username)), password(std::move(password)), role(std::move(role)) {}

    int id;

    std::string username;
    std::string password;

    std::string role;

    std::vector<Authorization> authorizations;
};

#endif //P8_WEB_SERVER_USER_ACCOUNT_H

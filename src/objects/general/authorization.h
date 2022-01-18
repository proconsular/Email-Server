//
// Created by Chris Luttio on 5/8/21.
//

#ifndef P8_WEB_SERVER_AUTHORIZATION_H
#define P8_WEB_SERVER_AUTHORIZATION_H

#include <string>
#include <chrono>

class Authorization {
public:

    std::string client_id;

    std::string access_token;
    std::string refresh_token;

    std::string ip;

    std::chrono::system_clock::time_point created_at;
};

#endif //P8_WEB_SERVER_AUTHORIZATION_H

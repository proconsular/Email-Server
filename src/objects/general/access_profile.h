//
// Created by Chris Luttio on 5/4/21.
//

#ifndef P8_WEB_SERVER_ACCESS_PROFILE_H
#define P8_WEB_SERVER_ACCESS_PROFILE_H

#include <string>
#include <utility>
#include <vector>

class AccessProfile {
public:
    AccessProfile() {}
    explicit AccessProfile(std::string name): name(std::move(name)), request_size_limit(10 * 1024) {}

    std::string name;
    std::vector<std::string> allowed_methods;
    uint32_t request_size_limit;
};

#endif //P8_WEB_SERVER_ACCESS_PROFILE_H

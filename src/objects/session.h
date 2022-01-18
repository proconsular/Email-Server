//
// Created by Chris Luttio on 5/16/21.
//

#ifndef P8_WEB_SERVER_SESSION_H
#define P8_WEB_SERVER_SESSION_H

#include <string>
#include <utility>
#include "utils.h"
#include "session_record.h"

class Session {
public:
    Session(std::string name, std::string ip): name(std::move(name)), ip(std::move(ip)) {
        _id = std::to_string(rand() * 1000000000000);
    }

    const std::string& id() const {
        return _id;
    }

    std::string name;
    std::string ip;

    std::vector<SessionRecord> records;
private:
    std::string _id;
};

#endif //P8_WEB_SERVER_SESSION_H

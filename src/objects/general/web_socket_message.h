//
// Created by Chris Luttio on 5/10/21.
//

#ifndef P8_WEB_SERVER_WEB_SOCKET_MESSAGE_H
#define P8_WEB_SERVER_WEB_SOCKET_MESSAGE_H

#include <iostream>
#include "general/connection.h"

class WebSocketMessage {
public:
    explicit WebSocketMessage(): sent(false) {}

    uint8_t opcode;
    std::shared_ptr<std::string> payload;

    std::shared_ptr<Connection> connection;

    bool sent;
};

#endif //P8_WEB_SERVER_WEB_SOCKET_MESSAGE_H

//
// Created by Chris Luttio on 5/17/21.
//

#ifndef P8_WEB_SERVER_WEB_SOCKET_MESSAGE_FRAME_H
#define P8_WEB_SERVER_WEB_SOCKET_MESSAGE_FRAME_H

#include <string>
#include <memory>

#include "connection.h"

class WebSocketMessageFrame {
public:
    uint8_t opcode;
    std::shared_ptr<std::string> payload;
};

#endif //P8_WEB_SERVER_WEB_SOCKET_MESSAGE_FRAME_H

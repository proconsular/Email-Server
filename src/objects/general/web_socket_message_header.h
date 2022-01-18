//
// Created by Chris Luttio on 5/17/21.
//

#ifndef P8_WEB_SERVER_WEB_SOCKET_MESSAGE_HEADER_H
#define P8_WEB_SERVER_WEB_SOCKET_MESSAGE_HEADER_H

#include <iostream>

class WebSocketMessageHeader {
public:
    WebSocketMessageHeader(): length(0) {}

    uint8_t opcode;
    uint64_t length;
    const char *mask;
    int payload_offset;

    bool finished;
    bool masked;
};

#endif //P8_WEB_SERVER_WEB_SOCKET_MESSAGE_HEADER_H

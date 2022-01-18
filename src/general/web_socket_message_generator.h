//
// Created by Chris Luttio on 5/10/21.
//

#ifndef P8_WEB_SERVER_WEB_SOCKET_MESSAGE_GENERATOR_H
#define P8_WEB_SERVER_WEB_SOCKET_MESSAGE_GENERATOR_H

#include <iostream>
#include "objects/general/web_socket_message.h"

class WebSocketMessageGenerator {
public:
    static std::vector<std::vector<char>> generate(const WebSocketMessage&);
};


#endif //P8_WEB_SERVER_WEB_SOCKET_MESSAGE_GENERATOR_H

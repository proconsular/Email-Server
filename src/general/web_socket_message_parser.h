//
// Created by Chris Luttio on 5/11/21.
//

#ifndef P8_WEB_SERVER_WEB_SOCKET_MESSAGE_PARSER_H
#define P8_WEB_SERVER_WEB_SOCKET_MESSAGE_PARSER_H

#include "objects/general/web_socket_message_frame.h"
#include "objects/general/web_socket_message_header.h"

class WebSocketMessageParser {
public:
    static WebSocketMessageHeader parse_header(const char*);

    static uint64_t get_length(const char *);
    static WebSocketMessageFrame parse(const char *, uint64_t);
};


#endif //P8_WEB_SERVER_WEB_SOCKET_MESSAGE_PARSER_H

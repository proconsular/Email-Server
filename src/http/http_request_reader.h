//
// Created by Chris Luttio on 5/1/21.
//

#ifndef P8_WEB_SERVER_HTTP_REQUEST_READER_H
#define P8_WEB_SERVER_HTTP_REQUEST_READER_H

#include "general/connection.h"
#include "objects/general/http_message.h"
#include "http_message_parser.h"

class HttpRequestReader {
public:
    explicit HttpRequestReader(int limit): _read_limit(limit), _parser(REQUEST), _read_limit_reached(false) {}

    int read(const std::shared_ptr<Connection>&);

    std::shared_ptr<HttpMessage> get_message() const {
        return _parser.get_message();
    }

    bool reached_limit() const {
        return _read_limit_reached;
    }
private:
    HttpMessageParser _parser;

    int _read_limit;
    bool _read_limit_reached;
};


#endif //P8_WEB_SERVER_HTTP_REQUEST_READER_H

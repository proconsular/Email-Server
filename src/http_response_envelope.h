//
// Created by Chris Luttio on 3/27/21.
//

#ifndef P8_WEB_SERVER_HTTP_RESPONSE_ENVELOPE_H
#define P8_WEB_SERVER_HTTP_RESPONSE_ENVELOPE_H

#include <utility>

#include "connection.h"
#include "http_response.h"

class HTTPResponseEnvelope {
public:
    HTTPResponseEnvelope(std::shared_ptr<Connection> conn, std::shared_ptr<HTTPResponse> response): connection(std::move(conn)), response(std::move(response)) {}

    std::shared_ptr<Connection> connection;
    std::shared_ptr<HTTPResponse> response;
};

#endif //P8_WEB_SERVER_HTTP_RESPONSE_ENVELOPE_H

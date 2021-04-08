//
// Created by Chris Luttio on 3/27/21.
//

#ifndef P8_WEB_SERVER_HTTP_REQUEST_ENVELOPE_H
#define P8_WEB_SERVER_HTTP_REQUEST_ENVELOPE_H

#include <utility>

#include "http_request.h"
#include "connection.h"

class HTTPRequestEnvelope {
public:
    HTTPRequestEnvelope(std::shared_ptr<Connection> conn, std::shared_ptr<HTTPRequest> http_request): connection(std::move(conn)), request(std::move(http_request)) {}

    std::shared_ptr<Connection> connection;
    std::shared_ptr<HTTPRequest> request;
};

#endif //P8_WEB_SERVER_HTTP_REQUEST_ENVELOPE_H

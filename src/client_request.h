//
// Created by Chris Luttio on 3/26/21.
//

#ifndef P8_WEB_SERVER_CLIENT_REQUEST_H
#define P8_WEB_SERVER_CLIENT_REQUEST_H

#include <iostream>
#include <map>

#include "connection.h"
#include "url.h"
#include "identifiable.h"
#include "utils.h"
#include "objects/http_message.h"
#include "objects/route.h"

enum Requests {
    Unsupported,
    BadRequest,
    RetrieveFile,
    ResolveRoute,
    RedirectSSL,
};

enum RequestStatus {
    New,
    Working,
    Complete,
    Failed,
};

class ClientRequest: public Identifiable {
public:
    explicit ClientRequest():
        type(Requests::BadRequest),
        status(RequestStatus::New),
        data(nullptr),
        connection(nullptr) {
        _id = generate_hash_id(10);
    }

    [[nodiscard]] std::string id() const override {
        return _id;
    }

    Requests type;
    RequestStatus status;

    std::shared_ptr<HttpMessage> http_request;
    std::map<std::string, std::string> response_headers;

    Route route;

    URL uri;
    URL path;
    std::shared_ptr<std::string> data;

    std::shared_ptr<Connection> connection;

private:
    std::string _id;
};


#endif //P8_WEB_SERVER_CLIENT_REQUEST_H

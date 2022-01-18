//
// Created by Chris Luttio on 3/26/21.
//

#ifndef P8_WEB_SERVER_CLIENT_REQUEST_H
#define P8_WEB_SERVER_CLIENT_REQUEST_H

#include <iostream>
#include <map>

#include "general/connection.h"
#include "general/url.h"
#include "general/identifiable.h"
#include "general/utils.h"
#include "objects/general/http_message.h"
#include "objects/general/route.h"

enum Requests {
    Unsupported,
    NotFound,
    BadRequest,
    RetrieveFile,
    ResolveRoute,
    RedirectSSL,
    Authorize,
    Forbidden,
    Refresh,
    WebSocketUpgrade,
    Created,
    SendMail,
    ListMail,
    GetMail,
    OK,
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

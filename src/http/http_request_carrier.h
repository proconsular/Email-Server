//
// Created by Chris Luttio on 3/31/21.
//

#ifndef P8_WEB_SERVER_HTTP_REQUEST_CARRIER_H
#define P8_WEB_SERVER_HTTP_REQUEST_CARRIER_H

#include <utility>

#include "objects/general/http_message.h"
#include "general/connection.h"
#include <memory>

enum HTTPRequestStatus {
    NEW,
    CONNECTED,
    SENT,
    FULFILLED,
    FAILED,
    EXHAUSTED,
};

class HTTPRequestCarrier {
public:
    explicit HTTPRequestCarrier(URL url, std::shared_ptr<HttpMessage> request): url(std::move(url)), http_request(std::move(request)), status(NEW) {
        connection = nullptr;
        _id = generate_hash_id(10);
        has_been_read = false;
        connection_attempts = 0;
        send_attempts = 0;
    }

    [[nodiscard]] std::string id() const {
        return _id;
    }

    HTTPRequestStatus status;
    std::shared_ptr<HttpMessage> http_request;
    std::shared_ptr<Connection> connection;

    std::shared_ptr<HttpMessage> http_response;

    std::chrono::high_resolution_clock::time_point initial_read_time;
    bool has_been_read;

    URL url;

    uint8_t connection_attempts;
    uint8_t send_attempts;

private:
    std::string _id;
};


#endif //P8_WEB_SERVER_HTTP_REQUEST_CARRIER_H

//
// Created by Chris Luttio on 4/14/21.
//

#ifndef P8_WEB_SERVER_HTTP_MESSAGE_H
#define P8_WEB_SERVER_HTTP_MESSAGE_H

#include "general/url.h"

#include <string>
#include <map>
#include <memory>

enum HttpMessageType {
    REQUEST,
    RESPONSE,
};

class HttpMessage {
public:
    explicit HttpMessage(HttpMessageType type): type(type), code(0) {}

    static std::shared_ptr<HttpMessage> make_request(const std::string& method, const std::string& url) {
        auto request = std::make_shared<HttpMessage>(REQUEST);
        request->version = "HTTP/1.1";
        request->method = method;
        request->url = URL::parse(url);
        return request;
    }

    HttpMessageType type;

    int code;
    std::string status;
    std::string method;
    std::string version;
    URL url;

    std::map<std::string, std::shared_ptr<std::string>> headers;
    std::shared_ptr<std::string> body;

    std::shared_ptr<std::string> generate() const;
};

#endif //P8_WEB_SERVER_HTTP_MESSAGE_H

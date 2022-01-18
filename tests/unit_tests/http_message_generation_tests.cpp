//
// Created by Chris Luttio on 4/22/21.
//

#include "gtest/gtest.h"
#include "objects/http_message.h"

TEST(http_message_generation_tests, response_generation) {
    auto ok = HttpMessage(RESPONSE);
    ok.status = "OK";
    ok.version = "HTTP/1.1";
    ok.code = 200;
    ok.body = std::make_shared<std::string>("Hello world");
    ok.headers["Content-Type"] = std::make_shared<std::string>("text/plain");
    ok.headers["Cache-Control"] = std::make_shared<std::string>("no-cache");
    ok.headers["Host"] = std::make_shared<std::string>("localhost:8080");
    ASSERT_EQ("HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nContent-Type: text/plain\r\nHost: localhost:8080\r\nContent-Length: 11\r\n\r\nHello world", *ok.generate());
}

TEST(http_message_generation_tests, request_generation) {
    auto request = HttpMessage(REQUEST);
    request.version = "HTTP/1.1";
    request.url = URL::parse("/");
    request.method = "GET";
    request.headers["Content-Type"] = std::make_shared<std::string>("text/plain");
    request.headers["Content-Encoding"] = std::make_shared<std::string>("hello");
    request.body = std::make_shared<std::string>("Hello world");
    ASSERT_EQ("GET / HTTP/1.1\r\nContent-Encoding: hello\r\nContent-Type: text/plain\r\nContent-Length: 11\r\n\r\nHello world", *request.generate());
}
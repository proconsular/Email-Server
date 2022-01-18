//
// Created by Chris Luttio on 5/1/21.
//

#include "gtest/gtest.h"
#include "http_message_parser.h"

TEST(http_message_parser_error_tests, status_request_line_method) {
    HttpMessageParser parser(REQUEST);

    auto data = "GET12231fdfwe / HTTP/1.1\r\n\r\n";

    ASSERT_FALSE(parser.parse(data, strlen(data)));
    ASSERT_TRUE(parser.error());
}

TEST(http_message_parser_error_tests, status_request_line_version) {
    HttpMessageParser parser(REQUEST);

    auto data = "GET / HTTP/1.13243133\r\n\r\n";

    ASSERT_FALSE(parser.parse(data, strlen(data)));
    ASSERT_TRUE(parser.error());
}

TEST(http_message_parser_error_tests, status_request_line_url) {
    HttpMessageParser parser(REQUEST);

    std::string data = "GET /";
    for (int i = 0; i < 511; i++) {
        data.append("a");
    }
    data.append(" HTTP/1.1\r\n\r\n");

    ASSERT_FALSE(parser.parse(data.c_str(), data.size()));
    ASSERT_FALSE(parser.error());

    HttpMessageParser parser2(REQUEST);

    std::string data2 = "GET /";
    for (int i = 0; i < 512; i++) {
        data2.append("a");
    }
    data2.append(" HTTP/1.1\r\n\r\n");

    ASSERT_FALSE(parser2.parse(data2.c_str(), data2.size()));
    ASSERT_TRUE(parser2.error());
}

TEST(http_message_parser_error_tests, message_headers_key_limit) {
    HttpMessageParser parser(REQUEST);

    std::string data = "GET / HTTP/1.1\r\nContent-Length";
    for (int i = 0; i < 17; i++) {
        data += "a";
    }
    data += ": 0\r\n\r\n";

    ASSERT_FALSE(parser.parse(data.c_str(), data.size()));
    ASSERT_TRUE(parser.error());
}

TEST(http_message_parser_error_tests, message_headers_value_limit) {
    HttpMessageParser parser(REQUEST);

    std::string data = "GET / HTTP/1.1\r\nContent-Length: ";
    for (int i = 0; i < 1025; i++) {
        data += "a";
    }
    data += "\r\n\r\n";

    ASSERT_FALSE(parser.parse(data.c_str(), data.size()));
    ASSERT_TRUE(parser.error());
}
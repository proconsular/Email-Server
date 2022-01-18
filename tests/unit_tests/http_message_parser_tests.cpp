//
// Created by Chris Luttio on 4/22/21.
//

#include "gtest/gtest.h"

#include "http_message_parser.h"

TEST(http_message_parser_tests, response_with_fixed_body) {
    HttpMessageParser parser(RESPONSE);

    auto response = parser.get_message();

    const char* buffer1 = "HTTP/1.1 200 OK\r\n";
    ASSERT_TRUE(parser.parse(buffer1, strlen(buffer1)));

    ASSERT_EQ(200, response->code);
    ASSERT_EQ("OK", response->status);
    ASSERT_EQ("HTTP/1.1", response->version);

    const char* buffer2 = "Content-Type: ";
    ASSERT_TRUE(parser.parse(buffer2, strlen(buffer2)));

    ASSERT_EQ(response->headers.end(), response->headers.find("Content-Type"));

    const char* buffer3 = "text/plain";
    ASSERT_TRUE(parser.parse(buffer3, strlen(buffer3)));

    ASSERT_EQ(nullptr, response->headers["Content-Type"]);

    const char* buffer4 = "\r\n";
    ASSERT_TRUE(parser.parse(buffer4, strlen(buffer4)));

    ASSERT_EQ("text/plain", *response->headers["Content-Type"]);

    const char* buffer5 = "Content-Length: 11\r\n\r\n";
    ASSERT_TRUE(parser.parse(buffer5, strlen(buffer5)));

    ASSERT_EQ("11", *response->headers["Content-Length"]);

    const char* buffer6 = "Hello";
    ASSERT_TRUE(parser.parse(buffer6, strlen(buffer6)));

    ASSERT_EQ(nullptr, response->body);

    const char* buffer7 = " world";
    ASSERT_FALSE(parser.parse(buffer7, strlen(buffer7)));

    ASSERT_EQ("Hello world", *response->body);

    const char* buffer8 = "bad";
    ASSERT_FALSE(parser.parse(buffer8, strlen(buffer8)));

    ASSERT_EQ("Hello world", *response->body);
}

TEST(http_message_parser_tests, response_with_chunked_body) {
    HttpMessageParser parser(RESPONSE);

    auto response = parser.get_message();

    const char* buffer1 = "HTTP/1.1 200 OK\r\n";
    ASSERT_TRUE(parser.parse(buffer1, strlen(buffer1)));

    ASSERT_EQ(200, response->code);
    ASSERT_EQ("OK", response->status);
    ASSERT_EQ("HTTP/1.1", response->version);

    const char* buffer2 = "Content-Type: ";
    ASSERT_TRUE(parser.parse(buffer2, strlen(buffer2)));

    ASSERT_EQ(response->headers.end(), response->headers.find("Content-Type"));

    const char* buffer3 = "text/plain";
    ASSERT_TRUE(parser.parse(buffer3, strlen(buffer3)));

    ASSERT_EQ(nullptr, response->headers["Content-Type"]);

    const char* buffer4 = "\r\n";
    ASSERT_TRUE(parser.parse(buffer4, strlen(buffer4)));

    ASSERT_EQ("text/plain", *response->headers["Content-Type"]);

    const char* buffer5 = "Transfer-Encoding: chunked\r\n\r\n";
    ASSERT_TRUE(parser.parse(buffer5, strlen(buffer5)));

    ASSERT_EQ("chunked", *response->headers["Transfer-Encoding"]);

    // Fusce feugiat eget odio ac mollis. Morbi vitae faucibus elit. Cras a ligula vitae mi imperdiet iaculis scelerisque

    const char* buffer6 = "12\r\n";
    ASSERT_TRUE(parser.parse(buffer6, strlen(buffer6)));

    ASSERT_EQ(nullptr, response->body);

    const char* buffer7 = "Fusce feugiat eget\r\n";
    ASSERT_TRUE(parser.parse(buffer7, strlen(buffer7)));

    ASSERT_EQ(nullptr, response->body);

    const char* buffer8 = "32\r\n";
    ASSERT_TRUE(parser.parse(buffer8, strlen(buffer8)));

    const char* buffer9 = " odio ac mollis. Morbi vitae faucibus elit. Cras a\r\n";
    ASSERT_TRUE(parser.parse(buffer9, strlen(buffer9)));

    const char* buffer10 = "2e\r\n";
    ASSERT_TRUE(parser.parse(buffer10, strlen(buffer10)));

    const char* buffer11 = " ligula vitae mi imperdiet iaculis scelerisque\r\n";
    ASSERT_TRUE(parser.parse(buffer11, strlen(buffer11)));

    ASSERT_EQ(nullptr, response->body);

    const char* buffer12 = "0\r\n";
    ASSERT_FALSE(parser.parse(buffer12, strlen(buffer12)));

    ASSERT_EQ("Fusce feugiat eget odio ac mollis. Morbi vitae faucibus elit. Cras a ligula vitae mi imperdiet iaculis scelerisque", *response->body);

    const char* buffer13 = "42130\r\n";
    ASSERT_FALSE(parser.parse(buffer13, strlen(buffer13)));

    ASSERT_EQ("Fusce feugiat eget odio ac mollis. Morbi vitae faucibus elit. Cras a ligula vitae mi imperdiet iaculis scelerisque", *response->body);
}

TEST(http_message_parser_tests, request_with_fixed_body) {
    HttpMessageParser parser(REQUEST);

    auto request = parser.get_message();

    const char* buffer1 = "GET / HTTP/1.1\r\n";
    ASSERT_TRUE(parser.parse(buffer1, strlen(buffer1)));

    ASSERT_EQ("GET", request->method);
    ASSERT_EQ("/", request->url.to_string());
    ASSERT_EQ("HTTP/1.1", request->version);

    const char* buffer2 = "Content-Type: ";
    ASSERT_TRUE(parser.parse(buffer2, strlen(buffer2)));

    ASSERT_EQ(request->headers.end(), request->headers.find("Content-Type"));

    const char* buffer3 = "text/plain";
    ASSERT_TRUE(parser.parse(buffer3, strlen(buffer3)));

    ASSERT_EQ(nullptr, request->headers["Content-Type"]);

    const char* buffer4 = "\r\n";
    ASSERT_TRUE(parser.parse(buffer4, strlen(buffer4)));

    ASSERT_EQ("text/plain", *request->headers["Content-Type"]);

    const char* buffer5 = "Content-Length: 11\r\n\r\n";
    ASSERT_TRUE(parser.parse(buffer5, strlen(buffer5)));

    ASSERT_EQ("11", *request->headers["Content-Length"]);

    const char* buffer6 = "Hello";
    ASSERT_TRUE(parser.parse(buffer6, strlen(buffer6)));

    ASSERT_EQ(nullptr, request->body);

    const char* buffer7 = " world";
    ASSERT_FALSE(parser.parse(buffer7, strlen(buffer7)));

    ASSERT_EQ("Hello world", *request->body);

    const char* buffer8 = "bad";
    ASSERT_FALSE(parser.parse(buffer8, strlen(buffer8)));

    ASSERT_EQ("Hello world", *request->body);
}

TEST(http_message_parser_tests, response_with_large_body) {
    FILE *response_file = fopen("../res/big-response-1.txt", "r");
    FILE *data_file = fopen("../res/data.txt", "r");
    char *resp_buffer = new char[10 * 1024];
    char *data_buffer = new char[10 * 1024];
    size_t ra = fread(resp_buffer, 1, 10 * 1024, response_file);
    size_t da = fread(data_buffer, 1, 10 * 1024, data_file);
    fclose(response_file);
    fclose(data_file);

    std::string data_str = std::string(data_buffer, data_buffer + da);

    delete[] data_buffer;

    HttpMessageParser parser(RESPONSE);

    ASSERT_FALSE(parser.parse(resp_buffer, ra));

    auto response = parser.get_message();

    ASSERT_EQ(200, response->code);
    ASSERT_EQ("OK", response->status);

    ASSERT_FALSE(response->body == nullptr);

    ASSERT_EQ(data_str.size(), response->body->size());
    ASSERT_EQ(data_str, *response->body);

    delete[] resp_buffer;
}
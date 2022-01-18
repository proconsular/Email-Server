//
// Created by Chris Luttio on 5/10/21.
//

#include "gtest/gtest.h"
#include "openssl/ssl.h"
#include "utils.h"

TEST(websocket_tests, test1) {
    std::string key = "dGhlIHNhbXBsZSBub25jZQ==";
    std::string input = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    uint8_t buffer[20];
    SHA1(reinterpret_cast<const uint8_t *>(input.c_str()), input.size(), buffer);

    std::string output = encode_base_64(buffer, 20);

    ASSERT_EQ("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", output);
}
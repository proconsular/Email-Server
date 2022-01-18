//
// Created by Chris Luttio on 5/11/21.
//

#include "web_socket_message_parser.h"
#include "utils.h"
#include "math.h"

WebSocketMessageFrame WebSocketMessageParser::parse(const char *data, uint64_t length) {
    bool fin = false;
    uint8_t opcode = 0;
    bool mask = false;
    uint32_t mask_key = 0;

    uint8_t b1 = data[0];
    uint8_t b2 = data[1];

    fin = b1 & 128;
    opcode = b1 & 127;
    mask = b2 & 128;

    uint8_t init_len = b2 & 127;
    uint8_t offset = 0;

    if (init_len == 126)
        offset += 2;
    else if (init_len == 127)
        offset += 8;

    uint64_t len = get_length(data);

    char buffer[4];
    memcpy(buffer, data + 2 + offset, 4);

    const char *masked_payload = data + 6 + offset;

    char *decoded_payload = new char[len];

    for (int i = 0; i < len; i++) {
        decoded_payload[i] = masked_payload[i] ^ buffer[i % 4];
    }

    WebSocketMessageFrame message;
    message.opcode = opcode;
    message.payload = std::make_shared<std::string>(decoded_payload, decoded_payload + len);

    delete[] decoded_payload;

    return message;
}

WebSocketMessageHeader WebSocketMessageParser::parse_header(const char *data) {
    WebSocketMessageHeader header;

    bool fin = false;
    uint8_t opcode = 0;
    bool mask = false;

    uint8_t b1 = data[0];
    uint8_t b2 = data[1];

    fin = b1 & 128;
    opcode = b1 & 127;
    mask = b2 & 128;

    uint8_t init_len = b2 & 127;
    uint8_t offset = 0;

    if (init_len == 126)
        offset += 2;
    else if (init_len == 127)
        offset += 8;

    uint64_t len = get_length(data);

    if (mask) {
        char *key = new char[4];
        memcpy(key, data + 2 + offset, 4);
        header.mask = key;
        offset += 4;
    }

    header.opcode = opcode;
    header.length = len;
    header.finished = fin;
    header.masked = mask;
    header.payload_offset = offset + 2;

    return header;
}

uint64_t WebSocketMessageParser::get_length(const char *data) {
    uint8_t first_block = data[1] & 127;
    if (first_block < 126)
        return first_block;
    std::vector<uint8_t> bytes;
    if (first_block == 126) {
        bytes.push_back(data[2]);
        bytes.push_back(data[3]);
    }
    if (first_block == 127)
        for (int i = 0; i < 8; i++)
            bytes.push_back(data[4 + i]);
    uint64_t length = 0;
    for (int i = 0; i < bytes.size(); i++)
        length += bytes[bytes.size() - i - 1] * pow(256, i);
    return length;
}
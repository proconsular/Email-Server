//
// Created by Chris Luttio on 5/10/21.
//

#include "web_socket_message_generator.h"
#include "math.h"

std::vector<std::vector<char>> WebSocketMessageGenerator::generate(const WebSocketMessage& message) {
    std::vector<std::vector<char>> frames;

    bool fin = true;

    char header[2] = {0, 0};
    header[0] = (fin << 7) | (message.opcode);

    int payload_type = 0;
    if (message.payload->size() < 126) {
        header[1] |= message.payload->size();
    } else if (message.payload->size() < 65536) {
        header[1] |= 126;
        payload_type = 1;
    } else {
        header[1] |= 127;
        payload_type = 2;
    }

    uint offset = 0;
    std::string bytes;
    uint64_t len = message.payload->size();

    if (payload_type == 1) {
        offset += 2;
        bytes.push_back((len >> 8) & 0xff);
        bytes.push_back(len & 0xff);
    } else if (payload_type == 2) {
        offset += 8;
        for (int i = 0; i < 8; i++) {
            bytes.push_back((len >> (8 * (8 - i - 1))) & 0xff);
        }
    }

    int size = 2 + offset + message.payload->size();
    char data[size];
    memcpy(data, header, 2);
    if (offset > 0)
        memcpy(data + 2, bytes.c_str(), bytes.size());
    memcpy(data + 2 + offset, message.payload->c_str(), message.payload->size());

    std::vector<char> frame(size);
    std::copy(data, data + size, frame.begin());

    frames.push_back(frame);

    return frames;
}
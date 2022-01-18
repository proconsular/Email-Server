//
// Created by Chris Luttio on 6/19/21.
//

#include "mail_identifier.h"
#include "general/utils.h"

#include <chrono>

std::string MailIdentifier::generate_id() {
    auto now = std::chrono::high_resolution_clock::now();
    auto time = now.time_since_epoch().count();
    srand(time);
    std::string output;
    for (int i = 0; i < 8; i++)
        output.push_back((time >> (i * 8)) & 0xff);
    output.push_back(rand());
    output.push_back(rand());
    output.push_back(rand());
    output.push_back(rand());
    std::string data = encode_base_64((uint8_t*)output.c_str(), output.size());
    for (int i = 0; i < data.size(); i++) {
        if (data[i] == '/') {
            data[i] = '_';
        }
    }
    return data;
}
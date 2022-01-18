//
// Created by Chris Luttio on 5/16/21.
//

#ifndef P8_WEB_SERVER_SESSION_RECORD_H
#define P8_WEB_SERVER_SESSION_RECORD_H

#include <iostream>
#include <string>
#include <chrono>

class SessionRecord {
public:

    std::chrono::high_resolution_clock::time_point timestamp;
    uint32_t scroll_height;
    uint32_t window_height;
    uint32_t document_height;
};

#endif //P8_WEB_SERVER_SESSION_RECORD_H

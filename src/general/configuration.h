//
// Created by Chris Luttio on 3/28/21.
//

#ifndef P8_WEB_SERVER_CONFIGURATION_H
#define P8_WEB_SERVER_CONFIGURATION_H

#include <iostream>
#include "url.h"

class Configuration {
public:
    Configuration() {
        port = 8000;
        keep_alive = 30;
        port_fixed = false;
        base_url = URL::parse("/");
        http_response_read_wait_ms = 10000;
        auto_reconnect_on_request_failure = true;
        auto_resend_on_request_failure = true;
        domain = "localhost";
    }

    URL base_url;
    int port;
    int keep_alive;

    uint32_t http_response_read_wait_ms;

    bool port_fixed;

    bool auto_reconnect_on_request_failure;
    bool auto_resend_on_request_failure;

    std::string from_file;
    std::string routing_directory;
    std::string domain;

    std::string mail_domain;

    std::string sender_domain;
};

#endif //P8_WEB_SERVER_CONFIGURATION_H

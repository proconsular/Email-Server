//
// Created by Chris Luttio on 4/17/21.
//

#ifndef P8_WEB_SERVER_DOCKER_INTERFACE_APP_H
#define P8_WEB_SERVER_DOCKER_INTERFACE_APP_H

#include <iostream>

#include "http_client_app.h"
#include "general/json.hpp"

class DockerInterfaceApp {
public:

    bool create_container(std::string&, const std::string&);
    bool create_container(std::string&, const std::string&, const std::string&, const std::string&);

    bool remove_container(const std::string&);
    bool remove_container_forced(const std::string&);

    bool start_container(const std::string&);
    bool stop_container(const std::string&);

    bool inspect_container(const std::string&, nlohmann::json&);
    bool get_container_status(const std::string&, std::string&);

    bool wait_for_container_status(const std::string&, const std::string&);

private:
    HttpClientApp http;
};


#endif //P8_WEB_SERVER_DOCKER_INTERFACE_APP_H

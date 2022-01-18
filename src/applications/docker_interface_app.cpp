//
// Created by Chris Luttio on 4/17/21.
//

#include "docker_interface_app.h"
#include "general/utils.h"
#include "general/json.hpp"

#include <thread>

using json = nlohmann::json;

const char *docker_sock = "unix:///var/run/docker.sock";

bool DockerInterfaceApp::create_container(std::string& id, const std::string &body) {
    auto request = HttpMessage::make_request("POST", "/containers/create");
    request->headers["Content-Type"] = std::make_shared<std::string>("application/json");
    request->body = std::make_shared<std::string>(body);
    std::shared_ptr<HttpMessage> response;
    if (http.send(docker_sock, request, response)) {
        if (response->code == 201 && response->body != nullptr) {
            json data = json::parse(*response->body);
            id = data["Id"];
            return true;
        }
    }
    return false;
}

bool DockerInterfaceApp::create_container(std::string& id, const std::string &image, const std::string &fromPort, const std::string &toPort) {
    auto request = HttpMessage::make_request("POST", "/containers/create");
    request->headers["Content-Type"] = std::make_shared<std::string>("application/json");
    auto p = string_format("%s/tcp", fromPort.c_str());
    json body = {
            {"Image",        image},
            {"ExposedPorts", {
                                     {p,              {}}
                             }},
            {"HostConfig",   {
                             {"PortBindings", {
                                                      {p, {
                                                                  {{"HostPort", toPort}}
                                                          }}
                                              }}
                     }}
    };
    request->body = std::make_shared<std::string>(body.dump());
    std::shared_ptr<HttpMessage> response;
    if (http.send(docker_sock, request, response)) {
        if (response->code == 201 && response->body != nullptr) {
            json data = json::parse(*response->body);
            id = data["Id"];
            return true;
        }
    }
    return false;
}

bool DockerInterfaceApp::remove_container(const std::string &id) {
    auto request = HttpMessage::make_request("DELETE", string_format("/containers/%s", id.c_str()));
    std::shared_ptr<HttpMessage> response;
    http.send(docker_sock, request, response);
    return response->code == 204;
}

bool DockerInterfaceApp::remove_container_forced(const std::string &id) {
    auto request = HttpMessage::make_request("DELETE", string_format("/containers/%s?force=true", id.c_str()));
    std::shared_ptr<HttpMessage> response;
    http.send(docker_sock, request, response);
    return response->code == 204;
}

bool DockerInterfaceApp::start_container(const std::string &id) {
    auto request = HttpMessage::make_request("POST", string_format("/containers/%s/start", id.c_str()));
    std::shared_ptr<HttpMessage> response;
    http.send(docker_sock, request, response);
    return response->code == 204;
}

bool DockerInterfaceApp::stop_container(const std::string &id) {
    auto request = HttpMessage::make_request("POST", string_format("/containers/%s/stop", id.c_str()));
    std::shared_ptr<HttpMessage> response;
    http.send(docker_sock, request, response);
    return response->code == 204;
}

bool DockerInterfaceApp::inspect_container(const std::string &id, json& object) {
    auto request = HttpMessage::make_request("GET", string_format("/containers/%s/json", id.c_str()));
    std::shared_ptr<HttpMessage> response;
    if (http.send(docker_sock, request, response)) {
        if (response->code == 200 && response->body != nullptr) {
            object = json::parse(*response->body);
            return true;
        }
    }

    return false;
}

bool DockerInterfaceApp::get_container_status(const std::string &id, std::string& status) {
    json info;
    if (inspect_container(id, info)) {
        status = info["State"]["Status"];
        return true;
    }
    return false;
}

bool DockerInterfaceApp::wait_for_container_status(const std::string &id, const std::string &status) {
    auto start_time = get_time();
    std::string current_status;
    while (current_status != status && get_millisecond_duration(get_time(), start_time) < 500) {
        get_container_status(id, current_status);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return true;
}
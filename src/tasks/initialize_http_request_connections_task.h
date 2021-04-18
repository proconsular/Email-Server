//
// Created by Chris Luttio on 4/1/21.
//

#ifndef P8_WEB_SERVER_INITIALIZE_HTTP_REQUEST_CONNECTIONS_TASK_H
#define P8_WEB_SERVER_INITIALIZE_HTTP_REQUEST_CONNECTIONS_TASK_H

#include "task.h"

#include <utility>
#include "state.h"
#include "controllers/controller.h"

class InitializeHTTPRequestConnectionsTask: public Task {
public:
    explicit InitializeHTTPRequestConnectionsTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): _state(std::move(state)), _controller(std::move(controller)), _alive(true) {}

    void perform() override;
    bool alive() override {
        return _alive;
    }
private:
    std::shared_ptr<State> _state;
    std::shared_ptr<Controller> _controller;
    bool _alive;

    static int create_ip_socket(const std::shared_ptr<HTTPRequestCarrier>&);
    static int create_unix_socket(const std::shared_ptr<HTTPRequestCarrier>&);
};


#endif //P8_WEB_SERVER_INITIALIZE_HTTP_REQUEST_CONNECTIONS_TASK_H

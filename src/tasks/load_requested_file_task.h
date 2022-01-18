//
// Created by Chris Luttio on 3/27/21.
//

#ifndef P8_WEB_SERVER_LOAD_REQUESTED_FILE_TASK_H
#define P8_WEB_SERVER_LOAD_REQUESTED_FILE_TASK_H

#include "task.h"

#include <utility>
#include "client_request.h"
#include "configuration.h"
#include "controllers/controller.h"

class LoadRequestedFileTask: public Task {
public:
    explicit LoadRequestedFileTask(std::shared_ptr<Controller> controller, std::shared_ptr<ClientRequest> request, std::shared_ptr<Configuration> config): _controller(std::move(controller)), _request(std::move(request)), config(std::move(config)), _alive(true) {}

    void perform() override;
    bool alive() override {
        return _alive;
    }

    std::shared_ptr<Configuration> config;
private:
    std::shared_ptr<Controller> _controller;
    std::shared_ptr<ClientRequest> _request;
    bool _alive;
};


#endif //P8_WEB_SERVER_LOAD_REQUESTED_FILE_TASK_H

//
// Created by Chris Luttio on 5/8/21.
//

#ifndef P8_WEB_SERVER_AUTHORIZE_USER_TASK_H
#define P8_WEB_SERVER_AUTHORIZE_USER_TASK_H

#include "task.h"

#include <utility>
#include "state.h"
#include "controllers/controller.h"

class AuthorizeUserTask: public Task {
public:
    explicit AuthorizeUserTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller, std::shared_ptr<ClientRequest> request): _state(std::move(state)), _controller(std::move(controller)), _request(std::move(request)), _alive(true) {}

    void perform() override;

    bool alive() override {
        return _alive;
    }
private:
    std::shared_ptr<ClientRequest> _request;
    std::shared_ptr<State> _state;
    std::shared_ptr<Controller> _controller;
    bool _alive;
};


#endif //P8_WEB_SERVER_AUTHORIZE_USER_TASK_H

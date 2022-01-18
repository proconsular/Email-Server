//
// Created by Chris Luttio on 4/1/21.
//

#ifndef P8_WEB_SERVER_RECEIVE_HTTP_RESPONSES_TASK_H
#define P8_WEB_SERVER_RECEIVE_HTTP_RESPONSES_TASK_H

#include "task.h"

#include <utility>
#include "state.h"
#include "controllers/controller.h"

class ReceiveHTTPResponsesTask: public Task {
public:
    explicit ReceiveHTTPResponsesTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): _state(std::move(state)), _controller(std::move(controller)), _alive(true) {}

    void perform() override;
    bool alive() override {
        return _alive;
    }
private:
    std::shared_ptr<State> _state;
    std::shared_ptr<Controller> _controller;
    bool  _alive;

};


#endif //P8_WEB_SERVER_RECEIVE_HTTP_RESPONSES_TASK_H

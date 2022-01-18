//
// Created by Chris Luttio on 7/25/21.
//

#ifndef P11_MAIL_SERVER_CONTROLLED_TASK_H
#define P11_MAIL_SERVER_CONTROLLED_TASK_H

#include <memory>
#include <utility>

#include "task.h"

#include "general/state.h"
#include "controllers/controller.h"

struct ControlledTask: Task {
    ControlledTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): _state(std::move(state)), _controller(std::move(controller)), _alive(true) {}

    bool alive() override { return _alive; }

protected:
    std::shared_ptr<State> _state;
    std::shared_ptr<Controller> _controller;

    bool _alive;
};

#endif //P11_MAIL_SERVER_CONTROLLED_TASK_H

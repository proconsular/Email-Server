//
// Created by Chris Luttio on 3/25/21.
//

#ifndef P8_WEB_SERVER_PRUNE_CONNECTIONS_TASK_H
#define P8_WEB_SERVER_PRUNE_CONNECTIONS_TASK_H

#include "tasks/task.h"

#include <utility>
#include "general/state.h"
#include "controllers/controller.h"

class PruneConnectionsTask: public Task {
public:
    PruneConnectionsTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): state(std::move(state)), _controller(std::move(controller)), _alive(true) {};

    void perform() override;

    bool alive() override {
        return _alive;
    }
private:
    std::shared_ptr<Controller> _controller;
    std::shared_ptr<State> state;
    bool _alive;
};


#endif //P8_WEB_SERVER_PRUNE_CONNECTIONS_TASK_H

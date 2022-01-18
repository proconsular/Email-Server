//
// Created by Chris Luttio on 4/24/21.
//

#ifndef P8_WEB_SERVER_LOAD_ROUTING_DATA_TASK_H
#define P8_WEB_SERVER_LOAD_ROUTING_DATA_TASK_H

#include "tasks/task.h"

#include <utility>

#include "general/state.h"

class LoadRoutingDataTask: public Task {
public:
    explicit LoadRoutingDataTask(std::shared_ptr<State> state): _state(std::move(state)), _alive(true) {};

    void perform() override;

    bool alive() override {
        return _alive;
    }
private:
    std::shared_ptr<State> _state;
    bool _alive;
};


#endif //P8_WEB_SERVER_LOAD_ROUTING_DATA_TASK_H

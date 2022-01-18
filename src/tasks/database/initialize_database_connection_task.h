//
// Created by Chris Luttio on 6/23/21.
//

#ifndef P11_MAIL_SERVER_INITIALIZE_DATABASE_CONNECTION_TASK_H
#define P11_MAIL_SERVER_INITIALIZE_DATABASE_CONNECTION_TASK_H

#include "tasks/task.h"
#include "general/state.h"
#include "controllers/controller.h"

class InitializeDatabaseConnectionTask: public Task {
public:
    InitializeDatabaseConnectionTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): _state(state), _controller(controller), _alive(true) {}

    void perform() override;
    bool alive() override {
        return _alive;
    }
private:
    std::shared_ptr<State> _state;
    std::shared_ptr<Controller> _controller;

    bool _alive;

    void setup_tables();
};


#endif //P11_MAIL_SERVER_INITIALIZE_DATABASE_CONNECTION_TASK_H

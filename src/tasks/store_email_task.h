//
// Created by Chris Luttio on 6/12/21.
//

#ifndef P11_MAIL_SERVER_STORE_EMAIL_TASK_H
#define P11_MAIL_SERVER_STORE_EMAIL_TASK_H

#include "task.h"
#include "state.h"
#include "controllers/controller.h"

#include "objects/email.h"

class StoreEmailTask: public Task {
public:
    explicit StoreEmailTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): _state(state), _controller(controller), _alive(true) {}

    void perform() override;
    bool alive() override { return _alive; }
private:
    std::shared_ptr<State> _state;
    std::shared_ptr<Controller> _controller;

    bool _alive;
};


#endif //P11_MAIL_SERVER_STORE_EMAIL_TASK_H

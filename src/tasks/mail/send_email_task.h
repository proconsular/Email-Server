//
// Created by Chris Luttio on 6/3/21.
//

#ifndef P11_MAIL_SERVER_SEND_EMAIL_TASK_H
#define P11_MAIL_SERVER_SEND_EMAIL_TASK_H

#include "tasks/task.h"

#include <utility>
#include "general/state.h"
#include "controllers/controller.h"

class SendEmailTask: public Task {
public:
    explicit SendEmailTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): _state(std::move(state)), _controller(controller), _alive(true) {}

    void perform() override;
    bool alive() override { return _alive; }
private:
    std::shared_ptr<State> _state;
    std::shared_ptr<Controller> _controller;

    bool _alive;

    std::vector<std::string> read_response(const std::shared_ptr<Connection>&, SSL*);
};


#endif //P11_MAIL_SERVER_SEND_EMAIL_TASK_H

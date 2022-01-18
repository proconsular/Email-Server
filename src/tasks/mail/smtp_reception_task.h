//
// Created by Chris Luttio on 6/12/21.
//

#ifndef P11_MAIL_SERVER_SMTP_RECEPTION_TASK_H
#define P11_MAIL_SERVER_SMTP_RECEPTION_TASK_H

#include "tasks/task.h"

#include <utility>
#include "general/state.h"
#include "controllers/controller.h"

class SmtpReceptionTask: public Task {
public:
    explicit SmtpReceptionTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): _state(std::move(state)), _controller(std::move(controller)), _alive(true) {}

    void perform() override;
    bool alive() override { return _alive; }
private:
    std::shared_ptr<State> _state;
    std::shared_ptr<Controller> _controller;

    bool _alive;
};


#endif //P11_MAIL_SERVER_SMTP_RECEPTION_TASK_H

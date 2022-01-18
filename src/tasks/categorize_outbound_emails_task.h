//
// Created by Chris Luttio on 6/5/21.
//

#ifndef P11_MAIL_SERVER_CATEGORIZE_OUTBOUND_EMAILS_TASK_H
#define P11_MAIL_SERVER_CATEGORIZE_OUTBOUND_EMAILS_TASK_H

#include "task.h"

#include <utility>
#include "state.h"

class CategorizeOutboundEmailsTask: public Task {
public:
    explicit CategorizeOutboundEmailsTask(std::shared_ptr<State> state): _state(std::move(state)), _alive(true) {}

    void perform() override;
    bool alive() override { return _alive; }
private:
    std::shared_ptr<State> _state;

    bool _alive;
};


#endif //P11_MAIL_SERVER_CATEGORIZE_OUTBOUND_EMAILS_TASK_H

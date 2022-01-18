//
// Created by Chris Luttio on 6/4/21.
//

#ifndef P11_MAIL_SERVER_RESOLVE_MAIL_CONNECTION_TASK_H
#define P11_MAIL_SERVER_RESOLVE_MAIL_CONNECTION_TASK_H

#include "tasks/task.h"

#include <utility>
#include "general/state.h"

class ResolveMailConnectionTask: public Task {
public:
    explicit ResolveMailConnectionTask(std::shared_ptr<State> state): _state(std::move(state)), _alive(true) {}

    void perform() override;
    bool alive() override { return _alive; }
private:
    std::shared_ptr<State> _state;

    bool _alive;
};


#endif //P11_MAIL_SERVER_RESOLVE_MAIL_CONNECTION_TASK_H

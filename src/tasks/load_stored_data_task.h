//
// Created by Chris Luttio on 6/23/21.
//

#ifndef P11_MAIL_SERVER_LOAD_STORED_DATA_TASK_H
#define P11_MAIL_SERVER_LOAD_STORED_DATA_TASK_H

#include "task.h"
#include "state.h"

class LoadStoredDataTask: public Task {
public:
    explicit LoadStoredDataTask(std::shared_ptr<State> state): _state(state), _alive(true) {}

    void perform() override;
    bool alive() override { return _alive; }
private:
    std::shared_ptr<State> _state;

    bool _alive;
};


#endif //P11_MAIL_SERVER_LOAD_STORED_DATA_TASK_H

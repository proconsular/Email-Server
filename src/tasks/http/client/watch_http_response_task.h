//
// Created by Chris Luttio on 4/15/21.
//

#ifndef P8_WEB_SERVER_WATCH_HTTP_RESPONSE_TASK_H
#define P8_WEB_SERVER_WATCH_HTTP_RESPONSE_TASK_H

#include "tasks/task.h"
#include "general/state.h"

#include <iostream>
#include <utility>
#include <functional>
#include <memory>

class WatchHttpResponseTask: public Task {
public:
    WatchHttpResponseTask(std::shared_ptr<State> state, std::string id, std::function<void (std::shared_ptr<HttpMessage>)> callback): _state(std::move(state)), _id(std::move(id)), _callback(std::move(callback)), _alive(true) {}

    void perform() override;

    bool alive() override {
        return _alive;
    }
private:
    std::shared_ptr<State> _state;

    std::string _id;
    std::function<void (std::shared_ptr<HttpMessage>)> _callback;

    bool _alive;
};


#endif //P8_WEB_SERVER_WATCH_HTTP_RESPONSE_TASK_H

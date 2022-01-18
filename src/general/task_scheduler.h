//
// Created by Chris Luttio on 3/26/21.
//

#ifndef P8_WEB_SERVER_TASK_SCHEDULER_H
#define P8_WEB_SERVER_TASK_SCHEDULER_H

#include <vector>

#include "tasks/task.h"

#include <memory>

class TaskScheduler {
public:
    TaskScheduler(): alive(true) {}

    void run();

    void add(const std::shared_ptr<Task>&);

    void enable() {
        alive = true;
    }

    void disable() {
        alive = false;
    }
private:
    std::vector<std::shared_ptr<Task>> tasks;

    bool alive;
};


#endif //P8_WEB_SERVER_TASK_SCHEDULER_H

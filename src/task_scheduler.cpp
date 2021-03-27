//
// Created by Chris Luttio on 3/26/21.
//

#include "task_scheduler.h"

void TaskScheduler::add(Task *task) {
    tasks.push_back(task);
}

void TaskScheduler::run() {
    while (true) {
        for (auto i = tasks.begin(); i != tasks.end(); i++) {
            (*i)->perform();
        }
    }
}
//
// Created by Chris Luttio on 3/26/21.
//

#include <iostream>
#include "task_scheduler.h"
#include <thread>

void TaskScheduler::add(const std::shared_ptr<Task>& task) {
    tasks.push_back(task);
}

void TaskScheduler::run() {
    while (alive) {
        for (auto i = tasks.begin(); i != tasks.end();) {
            auto task = *i;
            if (task->alive()) {
                task->perform();
                i++;
            } else {
                tasks.erase(i);
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds (100));
    }
}
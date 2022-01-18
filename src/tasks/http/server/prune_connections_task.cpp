//
// Created by Chris Luttio on 3/25/21.
//

#include "prune_connections_task.h"
#include <iostream>
#include <chrono>

void PruneConnectionsTask::perform() {
    for (auto iter = state->connections.cbegin(), next_iter = iter; iter != state->connections.cend(); iter = next_iter) {
        next_iter++;
        auto conn = iter->second;
        if (conn->protocol == Http) {
            auto time_since_read = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - conn->last_read);
            if (!conn->alive || (time_since_read.count() >= state->config->keep_alive && conn->active_requests == 0)) {
                _controller->apply(Action(RemoveClientConnection, conn));
            }
        } else {
            if (!conn->alive) {
                _controller->apply(Action(RemoveClientConnection, conn));
            }
        }
    }
}
//
// Created by Chris Luttio on 4/15/21.
//

#include "watch_http_response_task.h"

void WatchHttpResponseTask::perform() {
    auto carrier = _state->active_requests[_id];
    if (carrier->status == FULFILLED) {
        _callback(carrier->http_response);
        carrier->status = EXHAUSTED;
        _alive = false;
    } else if (carrier->status == FAILED) {
        _callback(nullptr);
        carrier->status = EXHAUSTED;
        _alive = false;
    }
}
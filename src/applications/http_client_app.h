//
// Created by Chris Luttio on 4/15/21.
//

#ifndef P8_WEB_SERVER_HTTP_CLIENT_APP_H
#define P8_WEB_SERVER_HTTP_CLIENT_APP_H

#include <utility>
#include <memory>

#include "general/state.h"

#include "controllers/direct_controller.h"
#include "actions/action.h"

#include "tasks/http/client/initialize_http_request_connections_task.h"
#include "tasks/http/client/send_http_requests_task.h"
#include "tasks/http/client/receive_http_responses_task.h"
#include "tasks/http/client/watch_http_response_task.h"

#include "receivers/log_action_receiver.h"

class HttpClientApp {
public:
    explicit HttpClientApp();

    bool send(const std::string&, const std::shared_ptr<HttpMessage>&, std::shared_ptr<HttpMessage>&);

    std::shared_ptr<State> state() const {
        return _state;
    }

    void enable_logging(const std::string& filename) {
        _controller->add_receiver(std::make_shared<LogActionReceiver>(filename));
    }
private:
    std::string _log_filename;

    std::shared_ptr<State> _state;
    std::shared_ptr<DirectController> _controller;
};


#endif //P8_WEB_SERVER_HTTP_CLIENT_APP_H

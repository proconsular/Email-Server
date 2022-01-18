//
// Created by Chris Luttio on 4/15/21.
//

#include "http_client_app.h"

HttpClientApp::HttpClientApp() {
    _state = std::make_shared<State>();
    _controller = std::make_shared<DirectController>(_state);

    _controller->apply(Action(StartApp, std::make_shared<std::string>("Http Client")));

    _state->scheduler->add(std::make_shared<InitializeHTTPRequestConnectionsTask>(_state, _controller));
    _state->scheduler->add(std::make_shared<SendHTTPRequestsTask>(_state, _controller));
    _state->scheduler->add(std::make_shared<ReceiveHTTPResponsesTask>(_state, _controller));
}

bool HttpClientApp::send(const std::string &domain, const std::shared_ptr<HttpMessage> &request, std::shared_ptr<HttpMessage>& out) {
    auto url = URL::parse(domain);
    request->headers["Host"] = std::make_shared<std::string>(url.domain_to_cstr());

    auto carrier = std::make_shared<HTTPRequestCarrier>(url, request);
    _controller->apply(Action(CreateOutboundHttpRequest, carrier));

    std::shared_ptr<HttpMessage> resp;

    auto trigger = [&resp, this](const std::shared_ptr<HttpMessage>& response) {
        resp = response;
        _state->scheduler->disable();
    };

    _state->scheduler->add(std::make_shared<WatchHttpResponseTask>(_state, carrier->id(), trigger));

    _state->scheduler->enable();
    _state->scheduler->run();

    if (resp != nullptr) {
        out = resp;
    }

    return resp != nullptr;
}
//
// Created by Chris Luttio on 6/12/21.
//

#include "initialize_smtp_server_task.h"
#include <thread>

void InitializeSmtpServerTask::perform() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(25);
    address.sin_addr.s_addr = INADDR_ANY;

    auto start_time = get_time();
    int err = 0;
    while ((err = bind(sock, (sockaddr *)&address, sizeof(address))) < 0 && get_ms_to_now(start_time) <= 2000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (err < 0) {
        _controller->apply(Action(ReportError, std::make_shared<std::string>("FAILED: SMTP initialization")));
        _alive = false;
        return;
    }

    if (listen(sock, 10) < 0) {
        _controller->apply(Action(ReportError, std::make_shared<std::string>("FAILED: SMTP initialization")));
        _alive = false;
        return;
    }

    fcntl(sock, F_SETFL, O_NONBLOCK);

    Socket socket(sock);
    socket.ip_address = address;
    socket.type = Socket::TCP;
    socket._port = 25;
    _state->mail->smtp_socket = socket;

    _controller->apply(Action(Print, std::make_shared<std::string>("STARTED: SMTP server: 25")));

    _alive = false;
}
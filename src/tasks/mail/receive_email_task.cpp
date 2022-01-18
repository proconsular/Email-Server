//
// Created by Chris Luttio on 6/12/21.
//

#include "receive_email_task.h"
#include "objects/mail/email_parser.h"

#include <algorithm>

enum EmailState {
    GREETING,
    EXTENSIONS,
    MAIL,
    RCPT,
    DATA,
    DONE,
    CLOSE,
    ERROR,
};

void ReceiveEmailTask::perform() {
    for (auto& connection: _state->mail->smtp_connections) {
        if (!connection->alive)
            continue;

        auto state = GREETING;
        auto sock = connection->socket.id;
        std::string domain;
        std::string from;
        std::vector<std::string> to;
        std::shared_ptr<std::string> data;

        auto read_time = get_time();

        bool open = true;
        while(open) {
            std::vector<std::string> response;

            switch (state) {
                case GREETING: {
                    response.emplace_back("220 drade.io");
                    break;
                }
                case EXTENSIONS: {
                    response.emplace_back("250 drade.io ready");
                    break;
                }
                case RCPT:
                case MAIL: {
                    response.emplace_back("250 OK");
                    break;
                }
                case DATA: {
                    response.emplace_back("354 Go ahead");
                    break;
                }
                case DONE: {
                    FILE* file = fopen("temp.txt", "w");
                    fwrite(data->c_str(), 1, data->size(), file);
                    fclose(file);
                    auto email = EmailParser::parse(data);
                    email->raw_message = std::make_unique<std::string>(*data);
                    email->status = Email::RECEIVED;
                    email->sender = from;
                    email->recipients = to;

                    _state->mail->append_receive_queue(email);
                    _controller->apply(Action(Print, std::make_shared<std::string>("RECEIVED: (" + email->name + ") Mail from: " + from)));

                    from = "";
                    to.clear();
                    data->clear();
                    domain = "";

                    response.emplace_back("250 OK");
                    break;
                }
                case CLOSE: {
                    response.emplace_back("221 drade.io closing");
                    break;
                }
                case ERROR: {
                    response.emplace_back("550 Protocol Error");
                    state = CLOSE;
                }
            }

            std::string output;
            for (const auto& line : response) {
                output.append(line + "\r\n");
            }
//            std::cout << output;
            write(sock, output.c_str(), output.size());

            switch (state) {
                case CLOSE: {
                    open = false;
                    close(sock);
                    connection->alive = false;
                    break;
                }
                case DATA: {
                    data = read_data(sock);
                    state = DONE;
                    break;
                }
                default: {
                    auto commands = read_commands(sock);
//                    for (const auto& c : commands)
//                        std::cout << c << std::endl;

                    if (commands.empty()) {
                        break;
                    }
                    auto first_command = commands[0];
                    int i = 0;
                    while (i < first_command.size() && first_command[i] != ' ') i++;
                    first_command = first_command.substr(0, i);

                    if (first_command == "EHLO") {
                        i = 5;
                        while (i < commands[0].size() && !isspace(commands[0][i])) i++;
                        domain = commands[0].substr(5, i);
//                        std::cout << domain << std::endl;
                        state = EXTENSIONS;
                    } else if (first_command == "MAIL") {
                        from = commands[0].substr(10);
                        state = MAIL;
                    } else if (first_command == "RCPT") {
                        to.emplace_back(commands[0].substr(8));
                        state = RCPT;
                    } else if (first_command == "DATA") {
                        state = DATA;
                    } else if (first_command == "QUIT") {
                        state = CLOSE;
                    } else {
                        state = ERROR;
                    }
                    break;
                }
            }
        }
    }
    for (const auto& c : _state->mail->smtp_connections) {
        if (c->alive) {
            close(c->socket.id);
            c->alive = false;
        }
    }
    _state->mail->smtp_connections.clear();
}

std::vector<std::string> ReceiveEmailTask::read_commands(int sock) {
    std::string command;
    const size_t len = 2 * 1024;
    char* buffer = new char[len];
    ssize_t amount = 0;
    auto start_time = get_time();
    while (get_ms_to_now(start_time) <= 2000) {
        amount = read(sock, buffer, len);
        if (amount > 0) {
            start_time = get_time();
            command += std::string(buffer, buffer + amount);
            if (command.size() >= 2) {
                if (command[command.size() - 2] == '\r' && command[command.size() - 1] == '\n') {
                    command.resize(command.size() - 2);
                    break;
                }
            }
        }
    }
    delete[] buffer;
    return {command};
}

std::shared_ptr<std::string> ReceiveEmailTask::read_data(int sock) {
    auto data = std::make_shared<std::string>();

    const ssize_t len = 10 * 1024;
    char* buffer = new char[len];
    ssize_t amount = 0;
    auto read_time = get_time();
    while (get_ms_to_now(read_time) <= 1000) {
        amount = read(sock, buffer, len);
        if (amount > 0) {
            read_time = get_time();
            data->append(std::string(buffer, buffer + amount));
            if (data->size() >= 5) {
                if (data->at(data->size() - 5) == '\r' &&
                    data->at(data->size() - 4) == '\n' &&
                    data->at(data->size() - 3) == '.' &&
                    data->at(data->size() - 2) == '\r' &&
                    data->at(data->size() - 1) == '\n') {
                    break;
                }
            }
        }
    }
    delete[] buffer;

    return data;
}
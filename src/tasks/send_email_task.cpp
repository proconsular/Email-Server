//
// Created by Chris Luttio on 6/3/21.
//

#include "send_email_task.h"

#include <unordered_set>
#include "actions/action.h"
#include <thread>

enum ConnectionState {
    INIT,
    TLS,
    SEND,
    CLOSE,
};

enum EmailState {
    MAIL,
    RCPT,
    DATA,
    DONE,
};

void SendEmailTask::perform() {
    for (auto& entry : _state->mail->send_domain_connections) {
        auto domain = entry.first;
        auto connection = entry.second;

        bool try_secure_connection = true;

        SSL* ssl = SSL_new(_state->mail->smtp_ssl_context);

        int ssl_err;
        ssl_err = SSL_set_fd(ssl, connection->socket.id);
        if (ssl_err <= 0) {
            std::cerr << "ssl failed\n";
            continue;
        }

        auto emails = _state->mail->send_domains[domain];

        auto state = INIT;
        bool secure = false;

        int current_email = 0;
        auto email_state = MAIL;
        std::vector<std::string> recipients;
        int current_recipient = 0;

        std::unordered_set<std::string> extensions;

        bool open = true;

        while (open) {
            std::vector<std::string> commands;

            std::vector<std::string> response;
            if (secure) {
                response = read_response(connection, ssl);
            } else {
                response = read_response(connection, nullptr);
            }

            if (response.empty()) {
                std::cerr << "no response from: " << domain << std::endl;
                break;
            }
//            for (auto& r : response)
//                std::cout << r;
//            std::cout << "\n";

            auto start = get_time();

            int code = atoi(response[0].substr(0, 3).c_str());
            std::string body = response[0].substr(4);

            switch (code) {
                case 421: {
                    open = false;
                    close(connection->socket.id);
                    break;
                }
            }

            while (commands.empty() && open) {
                if (get_ms_to_now(start) >= 2000) {
                    open = false;
                    std::cerr << "unknown response: " << response[0] << std::endl;
                }
                switch (state) {
                    case INIT: {

                        switch (code) {
                            case 220: {
                                commands.push_back("EHLO " + _state->config->domain);
                                break;
                            }
                            case 250: {
                                for (int i = 1; i < response.size(); i++) {
                                    auto line = response[i];
                                    extensions.insert(line.substr(4, line.size() - 6));
                                }
                                if (try_secure_connection && !secure && extensions.find("STARTTLS") != extensions.end()) {
                                    state = TLS;
                                    commands.emplace_back("STARTTLS");
                                } else {
                                    state = SEND;
                                }
                                break;
                            }
                        }

                        break;
                    }
                    case TLS: {

                        switch (code) {
                            case 220: {
                                auto start = get_time();
                                ssl_err = 0;
                                while ((ssl_err = SSL_connect(ssl)) <= 0 && get_ms_to_now(start) <= 500)
                                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                if (ssl_err <= 0) {
                                    std::cerr << "SSL connection err\n";
                                    state = CLOSE;
                                } else {
                                    state = INIT;
                                    secure = true;
                                }
                                break;
                            }
                        }

                        break;
                    }
                    case SEND: {
                        auto& email = emails[current_email];

                        switch (code) {
                            case 250: {
                                switch (email_state) {
                                    case MAIL:
                                        commands.push_back("MAIL FROM:<" + email->sender + ">");
                                        email_state = RCPT;
                                        for (auto& r : email->recipients) {
                                            int i = 0;
                                            while (r[i++] != '@');
                                            auto d = r.substr(i);
                                            if (d == domain)
                                                recipients.push_back(r);
                                        }
                                        break;
                                    case RCPT:
                                        commands.push_back("RCPT TO:<" + recipients[current_recipient] + ">");
                                        current_recipient++;
                                        if (current_recipient >= recipients.size())
                                            email_state = DATA;
                                        break;
                                    case DATA:
                                        commands.emplace_back("DATA");
                                        break;
                                    case DONE:
                                        email->status = Email::SENT;
                                        _controller->apply(Action(Print, std::make_shared<std::string>("SENT: Mail from: " + email->sender)));
                                        current_recipient = 0;
                                        recipients.clear();
                                        current_email++;
                                        if (current_email >= emails.size()) {
                                            state = CLOSE;
                                        } else {
                                            email_state = MAIL;
                                        }
                                        break;
                                }
                                break;
                            }
                            case 354: {
                                auto signature = _state->mail->email_signer.sign(email);
                                email->header->insert("DKIM-Signature", signature);
                                commands.push_back(email->generate() + "\r\n.");
                                email->header->erase("DKIM-Signature");
                                email_state = DONE;
                                break;
                            }
                            case 421: {
                                state = CLOSE;
                                email->status = Email::FAILED;
                                break;
                            }
                        }

                        break;
                    }
                    case CLOSE: {
                        switch (code) {
                            case 250: {
                                commands.push_back("QUIT");
                                break;
                            }
                            case 221: {
                                open = false;
                                close(connection->socket.id);
                                break;
                            }
                        }
                        break;
                    }
                }
            }

            if (open) {
                auto output = join("\r\n", commands) + "\r\n";
//                std::cout << output;
                if (secure) {
                    SSL_write(ssl, output.c_str(), output.size());
                } else {
                    write(connection->socket.id, output.c_str(), output.size());
                }
            }
        }
    }
    _state->mail->send_domains.clear();
    _state->mail->send_domain_connections.clear();
}

std::vector<std::string> SendEmailTask::read_response(const std::shared_ptr <Connection> &connection, SSL* ssl) {
    const size_t len = 1024;

    std::vector<std::string> lines;
    std::string line;
    bool complete = false;

    auto start = get_time();

    while (!complete) {
        if (get_ms_to_now(start) >= 10000) {
            std::cerr << "couldn't read response\n";
            break;
        }
        char *buffer = new char[len];
        ssize_t amount = 0;
        if (ssl == nullptr) {
            amount = read(connection->socket.id, buffer, len);
        } else {
            amount = SSL_read(ssl, buffer, len);
        }
        if (amount > 0) {
            line += std::string(buffer, buffer + amount);
            int i = 0;
            bool end = false;
            while (i < line.size()) {
                if (i == 3 && line[i] == ' ')
                    end = true;
                if (i > 1 && line[i] == '\n' && line[i - 1] == '\r') {
                    auto l = line.substr(0, i + 1);
                    lines.push_back(l);
                    line = line.substr(i + 1);
                    i = 0;
                    if (end) {
                        complete = true;
                        break;
                    }
                } else {
                    i++;
                }
            }
        }
        delete[] buffer;
    }

    return lines;
}

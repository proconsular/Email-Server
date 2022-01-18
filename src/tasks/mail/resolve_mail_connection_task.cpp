//
// Created by Chris Luttio on 6/4/21.
//

#include "resolve_mail_connection_task.h"
#include "udns.h"

const char* get_ip(in_addr_t addr) {
    char* str = new char[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);
    return str;
}

void ResolveMailConnectionTask::perform() {
    for (auto& entry : _state->mail->send_domains) {
        auto domain = entry.first;

        auto ctx = dns_new(nullptr);
        dns_open(ctx);

        auto mx_query = dns_resolve_mx(ctx, domain.c_str(), 0);

        if (mx_query != nullptr) {
            auto a4_query = dns_resolve_a4(ctx, mx_query->dnsmx_mx->name, 0);

            int sock = socket(AF_INET, SOCK_STREAM, 0);

            sockaddr_in address{};
            address.sin_addr.s_addr = a4_query->dnsa4_addr->s_addr;
            address.sin_family = AF_INET;
            address.sin_port = htons(25);

//            std::cout << get_ip(a4_query->dnsa4_addr->s_addr) << std::endl;

            if (connect(sock, (struct sockaddr*)&address, sizeof address) < 0) {
                std::cerr << "Connection to: (" << domain << ") failed.\n";
                continue;
            }

//            std::cout << "Connected to: (" + domain + ")\n";
            fcntl(sock, F_SETFL, O_NONBLOCK);

            auto connection = std::make_shared<Connection>();
            connection->_id = "1";
            connection->socket = Socket(sock);
            connection->socket.ip_address = address;
            connection->socket.type = Socket::TCP;
            connection->security = UNSECURE;
            connection->socket._port = 25;
            connection->protocol = Smtp;
            connection->last_read = get_time();

            _state->mail->send_domain_connections[domain] = connection;
        }
    }

}
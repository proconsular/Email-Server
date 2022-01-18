//
// Created by Chris Luttio on 3/23/21.
//

#ifndef P8_WEB_SERVER_SOCKET_H
#define P8_WEB_SERVER_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <memory>
#include <cstdio>
#include <cstring>
#include <sys/un.h>
#include <arpa/inet.h>

class Socket {
public:
    Socket() {}
    Socket(int id): id(id) {}

    int init();
    int bind();
    int listen(int);

    int get_error() const;

    int read(const std::shared_ptr<std::string>&) const;
    int write(const std::shared_ptr<std::string>&) const;

    bool accept(Socket&);

    void setup(int);

    int port() const {
        return _port != 0 ? _port : ntohs(ip_address.sin_port);
    }

    std::string ip() const {
        struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&ip_address;
        struct in_addr ipAddr = pV4Addr->sin_addr;

        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
        return str;
    }

    std::string get_socket() const {
        return ip() + ":" + std::to_string(port());
    }

    enum {
        UNIX,
        TCP,
        UDP,
    } type;

    struct sockaddr_in ip_address;
    struct sockaddr_un unix_address;

    int id;
    uint16_t _port;
private:

};

#endif //P8_WEB_SERVER_SOCKET_H

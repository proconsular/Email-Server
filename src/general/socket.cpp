//
// Created by Chris Luttio on 3/23/21.
//


#include "socket.h"
#include "constants.h"
#include <iostream>
#include <algorithm>
#include <math.h>

int Socket::init() {
    return id = socket(AF_INET, SOCK_STREAM, 0);
}

void Socket::setup(int port) {
    ip_address.sin_family = AF_INET;
    ip_address.sin_addr.s_addr = INADDR_ANY;
    ip_address.sin_port = htons(port);
    _port = port;
}

int Socket::bind() {
    return ::bind(id, (struct sockaddr*) &ip_address, sizeof (ip_address));
}

int Socket::listen(int amount) {
    return ::listen(id, amount);
}

bool Socket::accept(Socket& socket) {
    sockaddr_in client_addr{};
    socklen_t addrlen = sizeof(sockaddr_in);
    int client_id = ::accept(id, (struct sockaddr*) &client_addr, (socklen_t*) &addrlen);
    socket = Socket(client_id);
    socket.ip_address = client_addr;
    return client_id >= 0;
}

int Socket::read(const std::shared_ptr<std::string>& output) const {
    const int BUFFER_SIZE = 50 * KB;

    int amount_read = 0;
    do {
        char buffer[BUFFER_SIZE] = {0};
        amount_read = ::read(id, buffer, BUFFER_SIZE);
        output->append(buffer);
    } while (amount_read == BUFFER_SIZE);

    return amount_read;
}

int Socket::write(const std::shared_ptr<std::string>& input) const {
    const int BUFFER_SIZE = 50 * KB;
    const char *data = input->c_str();

    ssize_t amount_written;
    ssize_t cursor = 0;
    do {
        char buffer[BUFFER_SIZE] = {0};
        ssize_t size = fmin(input->size() - cursor, BUFFER_SIZE);
        memcpy(buffer, data + cursor, size);
        amount_written = ::write(id, buffer, size);
        if (amount_written == -1)
            return -1;
        cursor += amount_written;
    } while (amount_written == BUFFER_SIZE);

    return amount_written;
}

int Socket::get_error() const {
    int error_code;
    int error_code_size = sizeof(error_code);
    getsockopt(id, SOL_SOCKET, SO_ERROR, &error_code, reinterpret_cast<socklen_t *>(&error_code_size));
    return error_code;
}
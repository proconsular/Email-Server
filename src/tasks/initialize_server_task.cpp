//
// Created by Chris Luttio on 3/25/21.
//

#include "initialize_server_task.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <fcntl.h>

void InitializeServerTask::perform() {
    Socket sock;
    int port = 8000;

    while (true) {
        if (sock.init() == 0) {
            perror("Socket failed");
            exit(EXIT_FAILURE);
        }

        fcntl(sock.id(), F_SETFL, O_NONBLOCK);

        sock.setup(port);

        if (sock.bind() < 0) {
            port++;
            continue;
        }

        break;
    }

    std::cout << "INFO: Listening on port: " << port << std::endl;

    if (sock.listen(3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    state->server_socket = sock;
}
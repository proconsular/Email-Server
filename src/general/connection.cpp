//
// Created by Chris Luttio on 3/25/21.
//

#include "connection.h"
#include <math.h>
#include <thread>

bool Connection::read(const std::shared_ptr<std::string>& output) {
    int amount = socket.read(output);
    if (amount > 0) {
        last_read = std::chrono::high_resolution_clock::now();
    }
    return amount > 0;
}

bool Connection::write(const std::shared_ptr<std::string>& input) const {
    if (security == UNSECURE) {
        return socket.write(input) >= 0;
    } else {
        const int BUFFER_SIZE = fmin(50 * 1024, input->size());
        const char *data = input->c_str();

        auto last_wrote = get_time();

        int last_err = 0;

        int amount_written;
        int cursor = 0;
        do {
            char *buffer = new char[BUFFER_SIZE];
            int size = fmin(input->size() - cursor, BUFFER_SIZE);
            memcpy(buffer, data + cursor, size);
            amount_written = SSL_write(ssl, buffer, size);
            if (amount_written < 0) {
                int err = SSL_get_error(ssl, amount_written);
//                if (last_err != err) {
//                    std::cerr << "write err: " << err << std::endl;
//                }
                last_err = err;
                if (err == SSL_ERROR_WANT_WRITE) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                } else if (err == SSL_ERROR_WANT_READ) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            } else {
                last_wrote = get_time();
            }
            if (amount_written >= 0){
                cursor += amount_written;
            }
            delete[] buffer;
        } while (cursor < input->size() && get_ms_to_now(last_wrote) <= 1000);

        return cursor == input->size();
    }
}
//
// Created by Chris Luttio on 5/1/21.
//

#include "http_request_reader.h"


int HttpRequestReader::read(const std::shared_ptr<Connection> &connection) {
    const int buffer_size = 10 * 1024;

    auto last_read = get_time();

    bool still_reading = false;
    bool read_any = false;

    int data_read;
    int amount_read = 0;
    do {
        char *buffer = new char[buffer_size];

        if (connection->security == UNSECURE) {
            data_read = ::read(connection->socket.id, buffer, buffer_size);
        } else {
            data_read = SSL_read(connection->ssl, buffer, buffer_size);
        }

        if (data_read > 0) {
            amount_read += data_read;
            last_read = get_time();
            still_reading = _parser.parse(buffer, data_read);
            read_any = true;
        }

        delete[] buffer;
    } while (get_ms_to_now(last_read) <= 1000 && still_reading && (_read_limit == 0 || amount_read < _read_limit));

    if (!read_any)
        return 0;
    if (_parser.error())
        return -1;
    if (amount_read >= _read_limit) {
        _read_limit_reached = true;
        return -1;
    }

    _parser.finalize();
    return 1;
}
//
// Created by Chris Luttio on 5/10/21.
//

#include "recieve_web_socket_messages_task.h"
#include "web_socket_message_parser.h"

void RecieveWebSocketMessagesTask::perform() {
    for (auto& pair : _state->connections) {
        auto connection = pair.second;
        if (!connection->alive)
            break;
        if (connection->protocol != WebSocket)
            continue;
        auto data = std::make_shared<std::string>();

        const int buffer_size = 10 * 1024;

        auto last_read = get_time();

        bool read_any = false;
        bool big_frame = false;

        WebSocketMessageHeader header;

        int frame_count = 0;
        uint64_t frame_read = 0;

        int data_read;
        do {
            char *frame = new char[buffer_size];

            if (connection->security == UNSECURE) {
                data_read = ::read(connection->socket.id, frame, buffer_size);
            } else {
                data_read = SSL_read(connection->ssl, frame, buffer_size);
            }

            if (data_read > 0) {
                read_any = true;
                last_read = get_time();

                if (header.length > 0 && frame_read < header.length) {
                    data->append(frame, frame + data_read);
                    frame_read += data_read;
                    if (frame_read >= header.length) {
                        if (header.masked) {
                            for (int i = 0; i < data->size(); i++)
                                data->at(i) = data->at(i) ^ header.mask[i % 4];
                        }
                    }
                    delete[] frame;
                    continue;
                }

                frame_read = 0;

                WebSocketMessageHeader header1 = WebSocketMessageParser::parse_header(frame);

                if (header1.opcode == 8) {
                    connection->terminate();
                    break;
                }
                frame_count++;

                if (data_read < header1.length - 2) {
                    header = header1;
                    data->append(std::string(frame + header1.payload_offset, frame + data_read));
                    frame_read += data_read - header1.payload_offset;
                    big_frame = true;
                    continue;
                } else {
                    auto message_frame = WebSocketMessageParser::parse(frame, data_read);
                    data->append(*message_frame.payload);
                }

                header.length = 0;

                if (header1.finished) {
                    delete[] frame;
                    break;
                }
            }
            delete[] frame;
        } while (get_ms_to_now(last_read) <= 100);

        if (read_any) {
            WebSocketMessage message;
            message.opcode = header.opcode;
            message.payload = data;

            if (header.opcode == 9) {
                message.opcode = 0xA;
                _state->outbound_web_socket_messages.push_back(message);
            } else {
                _state->inbound_web_socket_messages.push_back(message);
            }
        }
    }
}
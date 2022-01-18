//
// Created by Chris Luttio on 4/1/21.
//

#include "receive_http_responses_task.h"
#include "general/constants.h"
#include "general/utils.h"

#include "http/http_message_parser.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>

void ReceiveHTTPResponsesTask::perform() {
    for (const auto& pair: _state->active_requests) {
        auto carrier = std::make_shared<HTTPRequestCarrier>(*pair.second);
        if (carrier->status == SENT) {
            if (!carrier->has_been_read) {
                carrier->initial_read_time = get_time();
                carrier->has_been_read = true;
            }

            const ssize_t buffer_size = 100 * KB;

            int socket = carrier->connection->socket.id;

            ssize_t amount = 0;

            HttpMessageParser parser(RESPONSE);

            auto was_read_time = get_time();

            bool did_read_any = false;

            do {
                char* buffer = new char[buffer_size];

                if (carrier->connection->security == UNSECURE) {
                    amount = read(socket, buffer, buffer_size);
                } else {
                    amount = SSL_read(carrier->connection->ssl, buffer, buffer_size);
                }

                if (amount > 0) {
                    was_read_time = get_time();
                    did_read_any = true;
                    if (!parser.parse(buffer, amount)) {
                        carrier->status = FULFILLED;
                        delete[] buffer;
                        break;
                    }
                } else {
                    if (get_ms_to_now(was_read_time) >= _state->config->http_response_read_wait_ms) {
                        delete[] buffer;
                        parser.finalize();
                        break;
                    }
                }

                delete[] buffer;
            } while (true);

            if (did_read_any) {
                carrier->status = FULFILLED;
                carrier->http_response = parser.get_message();
            } else {
                if (get_ms_to_now(carrier->initial_read_time) >= 15000) {
                    if (_state->config->auto_resend_on_request_failure && carrier->send_attempts < 3) {
                        carrier->send_attempts++;
                        carrier->status = NEW;
                        carrier->connection->terminate();
                        carrier->connection = nullptr;
                    } else {
                        carrier->status = FAILED;
                    }
                }
            }
            
            _controller->apply(Action(ReceiveHttpResponse, carrier));
        }
    }
}

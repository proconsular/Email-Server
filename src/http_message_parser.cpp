//
// Created by Chris Luttio on 4/14/21.
//

#include "http_message_parser.h"
#include "utils.h"

bool HttpMessageParser::parse(const char *buffer, size_t amount) {
    if (done) {
        return false;
    }

    built_up_message->append(std::string(buffer, buffer + amount));

    while (cursor < built_up_message->size() && mode != BODY) {
        switch (mode) {
            case STATUS: {
                if (type == REQUEST) {
                    int length =  parse_request_status_line(built_up_message->begin(), built_up_message->end(), message);
                    if (length == -1) {
                        _error = true;
                        return false;
                    }
                    cursor += length;
                } else {
                    cursor += parse_response_status_line(built_up_message->begin(), built_up_message->end(), message);
                }
                if (cursor < built_up_message->size() && built_up_message->at(cursor) == '\r') {
                    return false;
                }
                mode = HEADERS;
                break;
            }
            case HEADERS: {
                bool ok = parse_headers();
                if (_error)
                    return false;
                if (!ok)
                    return true;
                break;
            }
            case CHUNKED: {
                return parse_chunked_body();
            }
            default:
                return true;
        }
    }

    if (mode == BODY && cursor + body_length <= built_up_message->size()) {
        message->body = std::make_shared<std::string>(built_up_message->begin() + cursor, built_up_message->begin() + cursor + body_length);
        done = true;
        return false;
    }

    return true;
}

bool HttpMessageParser::parse_headers() {
    int length = parse_header(built_up_message->begin() + cursor, built_up_message->end(), message);
    if (length == -2) {
        _error = true;
        return false;
    }
    if (length == -1) {
        return false;
    }
    cursor += length;
    if (cursor < built_up_message->size()) {
        if (built_up_message->at(cursor) == '\r' || built_up_message->at(cursor) == '\n') {
            mode = BODY;
            if (message->headers.find("Content-Length") != message->headers.end()) {
                body_length = atoi(message->headers["Content-Length"]->c_str());
            } else if (message->headers.find("Transfer-Encoding") != message->headers.end()) {
                mode = CHUNKED;
            }
            if (built_up_message->at(cursor) == '\r') {
                cursor += 2;
            } else {
                cursor += 1;
            }
        }
    }
    return true;
}

bool HttpMessageParser::parse_chunked_body() {
    int size;
    int length = parse_chunk_header(built_up_message, cursor, size);
    if (length == -1) {
        return true;
    }
    if (size == 0) {
        message->body = built_up_body;
        done = true;
        return false;
    }
    if (cursor + length + size >= built_up_message->size()) {
        return true;
    }
    cursor += length;
    built_up_body->append(std::string(built_up_message->begin() + cursor, built_up_message->begin() + cursor + size));
    cursor += size + 2;
    return true;
}

int HttpMessageParser::parse_chunk_header(const std::shared_ptr<std::string>& str, int start, int &size) {
    auto end = start;
    while (end < str->size() && str->at(end) != '\r' && str->at(end) != '\0') end++;
    if (end >= str->size() || str->at(end)  != '\r')
        return -1;
    if (str->at(end)  == '\r' && end + 1 >= str->size())
        return -1;
    size = decode_hex_str(std::string(str->begin() + start, str->begin() + end));
    return end - start + 2;
}
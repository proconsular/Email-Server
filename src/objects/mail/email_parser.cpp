//
// Created by Chris Luttio on 6/12/21.
//

#include "email_parser.h"
#include <algorithm>

std::shared_ptr<Email> EmailParser::parse(const std::shared_ptr<std::string> &data) {
    auto email = std::make_shared<Email>();
    email->raw_message = std::make_unique<std::string>(*data);

    uint32_t i = 0;
    while (i < data->size()) {
        int s = i;
        while (i < data->size() && !isspace(data->at(i)) && data->at(i) != ':') i++;
        auto key = data->substr(s, i - s);
        while (i < data->size() && data->at(i) != ':') i++;
        i++;
        while (i < data->size() && isspace(data->at(i)) && data->at(i) != '\r') i++;
        std::string value = parse_header_value(i, data.get());
//        if (data->at(i) != '\r') {
//            while (i < data->size()) {
//                if (i + 3 < data->size()) {
//                    if (data->at(i) == '\r' && data->at(i + 1) == '\n') {
//                        if (data->at(i + 2) == ' ' || data->at(i + 2) == '\t') {
//                            i += 2;
//                            if (data->at(i) != '\t') {
//                                int k = i;
//                                while (i < data->size() && (data->at(i) == ' ' || data->at(i) == '\t')) i++;
//                                int d = i - k;
//                                if (d % 2 == 0)
//                                    value += ' ';
//                            }
//                            i++;
//                        } else {
//                            break;
//                        }
//                    }
//                }
//                s = i;
//                while (s < data->size() && isspace(data->at(s)) && data->at(s) != '\r') s++;
//                if (s + 3 < data->size()) {
//                    i = s;
//                    if (data->at(s) == '\r' &&
//                        data->at(s + 1) == '\n' &&
//                        (data->at(s + 2) != ' ' && data->at(s + 2) != '\t')) {
//                        break;
//                    }
//                }
//                if (i < data->size()) {
//                    value += data->at(i);
//                    i++;
//                }
//            }
//        }
        email->header->insert(key, value);
        i += 2;
        if (i < data->size() && data->at(i) == '\r')
            break;
    }
    email->header->normalize();
    email->header->parse();
    i += 2;
    if (i < data->size()) {
        auto n = data->size();
        if (n >= 5) {
            int m = 0;
            auto end = "\r\n.\r\n";
            while (data->at(m + n - 6) == end[m]) m++;
            if (m == 5)
                n -= 5;
        }
        email->body = parse_body(i - 2, n, email->header.get(), data.get());
    }

    return email;
}

std::unique_ptr<EmailBody> EmailParser::parse_body(uint32_t start, uint32_t end, const EmailHeader* header, const std::string *raw_message) {
    auto body = std::make_unique<EmailBody>();

    auto content_type = header->get_content_type_header();

    if (content_type->type != "multipart") {
        auto part = std::make_unique<BodyPart>();
        part->content = std::make_unique<std::string>(raw_message->substr(start, end - start));
        body->body_parts.push_back(std::move(part));
    } else {
        parse_body_part(start, header, raw_message, body->body_parts);
    }

    return body;
}

uint32_t EmailParser::parse_body_part(uint32_t start, const EmailHeader *parent_header, const std::string *raw_message, std::vector<std::unique_ptr<BodyPart>>& parts) {
    uint32_t end = raw_message->size();
    std::string boundary = parent_header->get_content_type_header()->get_attribute("boundary");
    std::string start_boundary = "\r\n--" + boundary;
    uint32_t idx = start;
    idx = find_start_boundary(idx, start_boundary, raw_message);
    bool is_end = false;
    while (idx < end && !is_end) {
        auto part_header = std::make_unique<EmailHeader>();
        auto part = std::make_unique<BodyPart>();
        while (idx < end) {
            if (idx + 1 < end && raw_message->at(idx) == '\r' && raw_message->at(idx + 1) == '\n')
                break;
            while (idx < end && raw_message->at(idx) == ' ') idx++;
            auto n = idx;
            while (idx < end && raw_message->at(idx) != ':' && raw_message->at(idx) != ' ') idx++;
            std::string name = raw_message->substr(n, idx - n);
            while (idx < end && raw_message->at(idx) != ':') idx++;
            idx++;
            std::string value;
            while (idx < end && raw_message->at(idx) == ' ') idx++;
            while (idx < end) {
                if (idx + 2 < end &&
                    raw_message->at(idx) == '\r' &&
                    raw_message->at(idx + 1) == '\n' &&
                    (raw_message->at(idx + 2) == ' ' || raw_message->at(idx + 2) == '\t')) {
                    idx += 2;
                    if (raw_message->at(idx) == ' ') {
                        uint32_t i = 0;
                        while (idx + i < end && raw_message->at(idx + i) == ' ') i++;
                        if (i % 2 == 1 && i != 1)
                            i--;
                        idx += i;
                    } else {
                        idx += 1;
                    }
                }
                if (idx + 1 < end &&
                    raw_message->at(idx) == '\r' &&
                    raw_message->at(idx + 1) == '\n')
                    break;
                value += raw_message->at(idx++);
            }
            part_header->insert(name, value);
            idx += 2;
        }
        part_header->normalize();
        part_header->parse();
        auto part_content_type = part_header->get_content_type_header();
        idx += 2;
        auto body_start = idx;
        auto body_end = idx;
        if (part_content_type) {
            if (part_content_type->type != "multipart") {
                while (idx < end) {
                    uint32_t i = 0;
                    while (i + idx < end && i < start_boundary.size() && raw_message->at(i + idx) == start_boundary[i]) i++;
                    if (i == start_boundary.size()) {
                        is_end = i + idx + 1 < end && raw_message->at(i + idx) == '-' && raw_message->at(i + idx + 1) == '-';
                        while (i + idx + 1 < end && raw_message->at(i + idx) == '\r' && raw_message->at(i + idx + 1) == '\n') i++;
                        body_end = idx;
                        idx += i;
                        break;
                    }
                    idx++;
                }
                idx += 1;
                part->content = std::make_unique<std::string>(raw_message->substr(body_start, body_end - body_start));
            } else {
                idx = parse_body_part(idx - 2, part_header.get(), raw_message, part->body_parts);
                idx = find_start_boundary(idx, start_boundary, raw_message);
            }
        }
        part->headers = std::move(part_header);
        parts.push_back(std::move(part));
    }
    return idx;
}

uint32_t EmailParser::find_start_boundary(uint32_t idx, const std::string &boundary, const std::string *raw_message) {
    uint32_t end = raw_message->size();
    while (idx < end) {
        uint32_t i = 0;
        while (i + idx < end && i < boundary.size() && raw_message->at(i + idx) == boundary[i]) i++;
        if (i == boundary.size()) {
            while (idx + i + 1 < end &&
                   raw_message->at(idx + i) != '\r' &&
                   raw_message->at(idx + i + 1) != '\n') i++;
            idx += i;
            break;
        }
        idx++;
    }
    idx += 2;
    return idx;
}

std::string EmailParser::parse_header_value(uint32_t& idx, const std::string *raw_message) {
    uint32_t end = raw_message->size();
    std::string value;
    while (idx < end) {
        if (idx + 2 < end &&
            raw_message->at(idx) == '\r' &&
            raw_message->at(idx + 1) == '\n' &&
            (raw_message->at(idx + 2) == ' ' || raw_message->at(idx + 2) == '\t')) {
            idx += 2;
            if (raw_message->at(idx) == ' ') {
                uint32_t i = 0;
                while (idx + i < end && raw_message->at(idx + i) == ' ') i++;
                if (i % 2 == 1 && i != 1)
                    i--;
                idx += i;
            } else {
                idx += 1;
            }
        }
        if (idx + 1 < end &&
              raw_message->at(idx) == '\r' &&
              raw_message->at(idx + 1) == '\n')
            break;
        value += raw_message->at(idx++);
    }
    return value;
}
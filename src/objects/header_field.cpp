//
// Created by Chris Luttio on 6/28/21.
//

#include "header_field.h"
#include <utility>
#include <vector>

std::vector<TagField> TaggedHeaderField::parse(const std::string &value) {
    std::vector<TagField> tags;

    int n = 0;
    while (n < value.size()) {
        while (n < value.size() && value[n] == ' ') n++;
        int m = n;
        while (n < value.size() && value[n] != ' ' && value[n] != '=') n++;
        std::string name = value.substr(m, n - m);
        while (n < value.size() && value[n] != '=') n++;
        n++;
        while (n < value.size() && value[n] == ' ') n++;
        m = n;
        while (n < value.size() && value[n] != ' ' && value[n] != ';') n++;
        tags.emplace_back(name, value.substr(m, n - m));
        while (n < value.size() && value[n] != ';') n++;
        n++;
    }

    return tags;
}

ContentTypeHeaderField ContentTypeHeaderField::parse(std::string name, const std::string &data) {
    ContentTypeHeaderField field;
    field.name = std::move(name);

    int cursor = 0;

    while (cursor < data.size() && data[cursor] == ' ') cursor++;
    int start = cursor;
    while (cursor < data.size() && data[cursor] != '/' && data[cursor] != ' ' && data[cursor] != ';') cursor++;
    field.type = data.substr(start, cursor - start);
    while (cursor < data.size() && data[cursor] != '/') cursor++;
    cursor++;
    while (cursor < data.size() && data[cursor] == ' ') cursor++;
    start = cursor;
    while (cursor < data.size() && data[cursor] != ' ' && data[cursor] != ';') cursor++;
    if (start < cursor)
        field.subtype = data.substr(start, cursor - start);
    while (cursor < data.size() && data[cursor] != ';') cursor++;
    cursor++;
    while (cursor < data.size()) {
        while (cursor < data.size() && data[cursor] == ' ') cursor++;
        start = cursor;
        while (cursor < data.size() && data[cursor] != '=' && data[cursor] != ' ') cursor++;
        std::string tag = data.substr(start, cursor - start);
        std::transform(tag.begin(), tag.end(), tag.begin(), [](char c) {
            return std::tolower(c);
        });
        while (cursor < data.size() && data[cursor] != '=') cursor++;
        cursor++;
        while (cursor < data.size() && data[cursor] == ' ') cursor++;
        start = cursor;
        bool quoted_string = cursor < data.size() && data[start] == '"';
        if (quoted_string) {
            cursor++;
            while (cursor < data.size() && data[cursor] != '"') cursor++;
            cursor++;
        } else {
            while (cursor < data.size() && data[cursor] != ' ' && data[cursor] != ';') cursor++;
        }
        if (cursor > data.size())
            break;
        std::string value = data.substr(start, cursor - start);
        if (start < cursor)
            field.attributes[tag] = value;
        if (value[0] == '"')
            value = value.substr(1, value.size() - 2);
//        std::transform(value.begin(), value.end(), value.begin(), [](char c) {
//            return std::tolower(c);
//        });
        field.attributes[tag] = value;
        while (cursor < data.size() && data[cursor] != ';') cursor++;
        cursor++;
    }

    return field;
}

std::string EmailAddressHeaderField::generate() const {
    std::string output;

    for (const auto& address : value) {
        if (!address.name.empty())
            output += "\"" + address.name + "\" ";
        output += "<" + address.local + "@" + address.domain + ">";
        output += ", ";
    }
    output.resize(output.size() - 2);

    return output;
}

std::string TaggedHeaderField::generate() const {
    std::string output;

    for (const auto& tag : value) {
        output += tag.name + "=" + tag.value + "; ";
    }
    output.resize(output.size() - 2);

    return output;
}

std::string ContentTypeHeaderField::generate() const {
    std::string output;

    output += type + "/" + subtype;
    output += "; ";
    for (const auto& [key, value]: attributes)
        output += key + "=" + "\"" + value + "\"; ";
    output.resize(output.size() - 2);

    return output;
}
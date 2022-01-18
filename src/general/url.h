//
// Created by Chris Luttio on 3/28/21.
//

/*
 *
 * Requirements:
 *
 *
 */

#ifndef P8_WEB_SERVER_URL_H
#define P8_WEB_SERVER_URL_H

#include <iostream>
#include <vector>
#include <map>

class URL {
public:
    static URL parse(const std::string&);
    static URL append(const URL&, const URL&);

    [[nodiscard]] std::string to_string() const;

    const char* domain_to_cstr() const;
    const char* get_path_url() const;

    void prepend(const std::string&);
    void append(const std::string&);

    std::vector<std::string> domain;

    std::vector<std::string> components;
    std::map<std::string, std::string> query;
    std::string protocol;
    std::string extension;

    bool is_root;
    bool is_filepath;

    int port;
};


#endif //P8_WEB_SERVER_URL_H

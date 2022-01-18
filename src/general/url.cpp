//
// Created by Chris Luttio on 3/28/21.
//

#include "url.h"
#include "utils.h"
#include <algorithm>
#include <string.h>

URL URL::parse(const std::string &str) {
    auto url = URL();

    if (str.empty()) {
        return url;
    }

    if (str == "/") {
        url.is_root = true;
        return url;
    }

    int slash_count = 0;
    for (char iter : str) {
        if (iter == '/')
            slash_count++;
        else
            slash_count = 0;
        if (slash_count == 3) {
            break;
        }
    }
    if (slash_count == 3)
        url.is_filepath = true;

    url.is_root = str[0] == '/';

    auto components = split_string("/", str);
    if (str[0] != '/') {
        if (components[0].find(':') != std::string::npos) {
            url.protocol = components[0].substr(0, components[0].size() - 1);
            components.erase(components.begin());
        }
    }

    for (auto component: components) {
        if (!component.empty()) {
            url.components.push_back(component);
        }
    }

    if (!url.components.empty() && !url.protocol.empty()) {
        auto first = url.components[0];
        auto comps = split_string(".", first);
        auto f = split_string(":", comps.back());
        if (f.size() > 1) {
            url.port = atoi(f[1].c_str());
        }
        comps[comps.size() - 1] = f[0];
        std::reverse(comps.begin(), comps.end());
        url.domain = comps;
        url.components.erase(url.components.begin());
    }

    if (!url.components.empty()) {
        auto last = *(url.components.end() - 1);
        if (last.find('.') != std::string::npos) {
            auto parts = split_string(".", last);
            if (parts.size() > 1) {
                url.components[url.components.size() - 1] = join(".", std::vector(parts.begin(), parts.end() - 1));
                url.extension = parts[parts.size() - 1];
            }
        }
    }

    return url;
}

URL URL::append(const URL &base, const URL &component) {
    auto url = URL();

    url.is_root = base.is_root;
    url.components = std::vector(base.components);
    url.domain = std::vector(base.domain);
    url.protocol = base.protocol;

    for (auto c: component.components) {
        url.components.push_back(c);
    }

    url.extension = component.extension;

    return url;
}

std::string URL::to_string() const {
    std::string output;

    if (is_root) {
        output += "/";
    }

    if (!protocol.empty()) {
        output += protocol + "://";
        if (is_filepath)
            output += "/";
    }

    if (!domain.empty()) {
        std::vector<std::string> dp;
        for (auto iter = domain.rbegin(); iter != domain.rend(); iter++) {
            dp.push_back(*iter);
        }
        output += join(".", dp);
        if (port != 0) {
            output += ":" + std::to_string(port);
        }
        output += "/";
    }

    if (!components.empty()) {
        output += join("/", components);
    }

    if (!extension.empty()) {
        output += "." + extension;
    }

    return output;
}

const char* URL::domain_to_cstr() const {
    auto cpy_domain = std::vector(domain);
    std::reverse(cpy_domain.begin(), cpy_domain.end());
    auto str = join(".", cpy_domain);
    char* data = new char[str.size() + 1];
    memcpy(data, str.c_str(), str.size());
    data[str.size()] = '\0';
    return data;
}

const char* URL::get_path_url() const {
    auto str = join("/", components);
    str.insert(0, "/");
    char *buffer = new char[str.size()];
    memcpy(buffer, str.c_str(), str.size());
    return buffer;
}

void URL::prepend(const std::string &str) {
    components.insert(components.begin(), str);
}

void URL::append(const std::string &str) {
    components.push_back(str);
}
//
// Created by Chris Luttio on 4/6/21.
//

#ifndef P8_WEB_SERVER_IDENTIFIABLE_H
#define P8_WEB_SERVER_IDENTIFIABLE_H

class Identifiable {
public:
    [[nodiscard]] virtual std::string id() const = 0;
};

#endif //P8_WEB_SERVER_IDENTIFIABLE_H

#pragma once

#include <stdint.h>

struct Request {
    enum Type : uint8_t {
        None = 0,
        Launch = 1,
    } type;
    uint8_t iWidget;
};

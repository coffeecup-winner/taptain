#pragma once

#include <stdint.h>

struct Message {
    enum Type : uint8_t {
        None = 0,
        OK,
        Error, // If the type is Error, the message is followed by a newline-terminated string
        Launch,
        Pause,
        Resume,
        Cancel,
    } type;
    uint8_t iWidget;
};

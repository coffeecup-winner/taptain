#pragma once

#include <LCDWIKI_KBV.h>

#include <stdint.h>

class Widget {
    static constexpr uint16_t MAX_TEXT_LEN = 21;

    const uint16_t m_x;
    const uint16_t m_y;
    const uint16_t m_width;
    const uint16_t m_height;
    char m_name[MAX_TEXT_LEN];

public:
    Widget(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height)
        : m_x(x)
        , m_y(y)
        , m_width(width)
        , m_height(height)
        , m_name()
    { }

    void SetName(const char* name);

    void Draw(const LCDWIKI_KBV& lcd);
};

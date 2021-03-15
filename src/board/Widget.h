#pragma once

#include <LCDWIKI_KBV.h>

#include <stdint.h>

#include "Request.h"

class Widget {
    static constexpr uint16_t MAX_TEXT_LEN = 21;

    const uint16_t m_x;
    const uint16_t m_y;
    const uint16_t m_width;
    const uint16_t m_height;
    char m_name[MAX_TEXT_LEN];
    enum class State : uint8_t {
        Default = 0,
        Active,
    } m_state;
    enum DrawFlags : uint8_t {
        None = 0x00,
        Border = 0x01,
        Text = 0x02,
        All = 0xff,
    } m_drawFlags;

public:
    Widget(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height)
        : m_x(x)
        , m_y(y)
        , m_width(width)
        , m_height(height)
        , m_name()
        , m_state(State::Default)
        , m_drawFlags(DrawFlags::All)
    { }

    void Reset();
    void SetName(const char* name);

    void Tap(Request* request);
    void Draw(const LCDWIKI_KBV& lcd);
};

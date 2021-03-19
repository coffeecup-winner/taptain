#pragma once

#include <LCDWIKI_KBV.h>

#include <stdint.h>

#include "Message.h"

class Widget {
    static constexpr uint16_t MAX_TEXT_LEN = 21;

    const uint16_t m_x;
    const uint16_t m_y;
    const uint8_t m_width;
    const uint8_t m_height;
    char m_name[MAX_TEXT_LEN];
    uint8_t m_prevPercentProgress;
    uint8_t m_percentProgress;
    enum class State : uint8_t {
        Default = 0,
        Active,
        Paused,
    } m_state;
    enum DrawFlags : uint8_t {
        None = 0x00,
        Border = 0x01,
        Text = 0x02,
        Progress = 0x04,
        All = 0xff,
    } m_drawFlags;

public:
    Widget(const uint16_t x, const uint16_t y, const uint8_t width, const uint8_t height)
        : m_x(x)
        , m_y(y)
        , m_width(width)
        , m_height(height)
        , m_name()
        , m_prevPercentProgress(0)
        , m_percentProgress(0)
        , m_state(State::Default)
        , m_drawFlags(DrawFlags::All)
    { }

    void Reset();
    void SetName(const char* name);
    void SetProgress(const uint8_t percent);

    void Tap(Message* message);
    void TapElsewhere(Message *message);
    void Draw(const LCDWIKI_KBV& lcd);
};

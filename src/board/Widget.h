#pragma once

#include <LCDWIKI_KBV.h>

#include <stdint.h>

#include "Message.h"

class Widget {
    static constexpr uint16_t MAX_TEXT_LEN = 21;

    uint16_t m_x;
    uint16_t m_y;
    uint8_t m_width;
    uint8_t m_height;
    char m_name[MAX_TEXT_LEN];
    uint8_t m_prevPercentProgress;
    uint8_t m_percentProgress;
    enum class Type : uint8_t {
        Display = 0,
        Task,

        COUNT,
    } m_type;
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
    Widget()
        : m_x()
        , m_y()
        , m_width()
        , m_height()
        , m_name()
        , m_prevPercentProgress()
        , m_percentProgress()
        , m_type()
        , m_state()
        , m_drawFlags()
    { }

    Widget(const uint16_t x, const uint16_t y, const uint8_t width, const uint8_t height)
        : m_x(x)
        , m_y(y)
        , m_width(width)
        , m_height(height)
        , m_name()
        , m_prevPercentProgress(0)
        , m_percentProgress(0)
        , m_type(Type::Display)
        , m_state(State::Default)
        , m_drawFlags(DrawFlags::All)
    { }

    void Reset();
    void SetType(const uint8_t type);
    void SetName(const char* name);
    void SetProgress(const uint8_t percent);

    void Tap(Message* message);
    void TapElsewhere(Message *message);
    void Draw(const LCDWIKI_KBV& lcd);
};

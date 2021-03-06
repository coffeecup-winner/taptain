#include "Widget.h"

#include <string.h>

#include "Config.h"
#include "Colors.h"
#include "Error.h"

constexpr uint16_t TEXT_WIDTH = 5;
constexpr uint16_t TEXT_HEIGHT = 7;

void Widget::Reset()
{
    m_state = State::Default;
    m_drawFlags = DrawFlags::All;
}

void Widget::SetType(const uint8_t type)
{
    assert(type < (uint8_t)Type::COUNT);
    m_type = (Type)type;
    // We expect this to be set once on init and never change, so not setting any draw flags
}

void Widget::SetName(const char* name)
{
    strncpy(m_name, name, MAX_TEXT_LEN);
    m_drawFlags |= DrawFlags::Text;
}

void Widget::SetProgress(const uint8_t percent)
{
    m_prevPercentProgress = m_percentProgress;
    m_percentProgress = percent;
    m_drawFlags |= DrawFlags::Progress;
}

void Widget::Tap(Message* message)
{
    if (m_type == Type::Display) {
        return;
    }
    switch (m_state) {
        case State::Default:
            m_state = State::Active;
            m_prevPercentProgress = m_percentProgress;
            m_percentProgress = 0;
            m_drawFlags |= DrawFlags::Border | DrawFlags::Progress;
            message->type = Message::Type::Launch;
            break;
        case State::Active:
            if (m_percentProgress == 100) {
                m_state = State::Default;
                m_prevPercentProgress = m_percentProgress;
                m_percentProgress = 0;
                m_drawFlags |= DrawFlags::Border | DrawFlags::Progress;
            } else {
                m_state = State::Paused;
                m_drawFlags |= DrawFlags::Border;
                message->type = Message::Type::Pause;
            }
            break;
        case State::Paused:
            m_state = State::Default;
            m_prevPercentProgress = m_percentProgress;
            m_percentProgress = 0;
            m_drawFlags |= DrawFlags::Border | DrawFlags::Progress;
            message->type = Message::Type::Cancel;
            break;
        default:
            Error("Unknown state");
            break;
    }
}

void Widget::TapElsewhere(Message *message)
{
    if (m_type == Type::Display) {
        return;
    }
    switch (m_state) {
        case State::Default:
        case State::Active:
            // Do nothing
            break;
        case State::Paused:
            m_state = State::Active;
            m_drawFlags |= DrawFlags::Border;
            message->type = Message::Type::Resume;
            break;
        default:
            Error("Unknown state");
            break;
    }
}

void Widget::Draw(const LCDWIKI_KBV& lcd)
{
    if (m_drawFlags == DrawFlags::None) {
        return;
    }

    constexpr uint16_t BORDER_PADDING = 3;
    constexpr uint16_t BORDER_WIDTH = 7;

    if (m_drawFlags & DrawFlags::Border) {
        uint16_t borderColor;
        switch (m_state) {
            case State::Default:
                borderColor = COLOR_BLUE;
                break;
            case State::Active:
                borderColor = COLOR_GREEN;
                break;
            case State::Paused:
                borderColor = COLOR_ORANGE;
                break;
            default:
                Error("Unknown state");
                break;
        }
        lcd.Fill_Rect(
            m_x + BORDER_PADDING,
            m_y + BORDER_PADDING,
            m_width - BORDER_PADDING * 2,
            BORDER_WIDTH,
            borderColor);
        lcd.Fill_Rect(
            m_x + BORDER_PADDING,
            m_y + m_height - BORDER_PADDING - BORDER_WIDTH,
            m_width - BORDER_PADDING * 2,
            BORDER_WIDTH,
            borderColor);
        lcd.Fill_Rect(
            m_x + BORDER_PADDING,
            m_y + BORDER_PADDING + BORDER_WIDTH,
            BORDER_WIDTH,
            m_height - BORDER_PADDING * 2 - BORDER_WIDTH * 2,
            borderColor);
        lcd.Fill_Rect(
            m_x + m_width - BORDER_PADDING - BORDER_WIDTH,
            m_y + BORDER_PADDING + BORDER_WIDTH,
            BORDER_WIDTH,
            m_height - BORDER_PADDING * 2 - BORDER_WIDTH * 2,
            borderColor);
    }

    if (m_drawFlags & DrawFlags::Progress && m_prevPercentProgress != m_percentProgress)
    {
        uint8_t from, to;
        if (m_prevPercentProgress < m_percentProgress) {
            from = m_prevPercentProgress;
            to = m_percentProgress;
            lcd.Set_Draw_color(COLOR_BACKGROUND_HIGHLIGHT);
        } else {
            from = m_percentProgress;
            to = m_prevPercentProgress;
            lcd.Set_Draw_color(COLOR_BACKGROUND);
        }
        lcd.Fill_Rectangle(
            m_x + BORDER_PADDING * 2 + BORDER_WIDTH + map(from, 0, 100, 0, m_width - BORDER_PADDING * 4 - BORDER_WIDTH * 2),
            m_y + BORDER_PADDING * 2 + BORDER_WIDTH,
            m_x + BORDER_PADDING * 2 + BORDER_WIDTH + map(to, 0, 100, 0, m_width - BORDER_PADDING * 4 - BORDER_WIDTH * 2) - 1,
            m_y + m_height - BORDER_PADDING * 2 - BORDER_WIDTH - 1
        );
    }

    if (m_drawFlags & (DrawFlags::Text | DrawFlags::Progress)) {
        if (!(m_drawFlags & DrawFlags::Progress)) {
            lcd.Fill_Rect(
                m_x + BORDER_PADDING * 2 + BORDER_WIDTH,
                m_y + m_height - BORDER_PADDING * 2 - BORDER_WIDTH - TEXT_HEIGHT,
                m_width - BORDER_WIDTH * 2 - BORDER_PADDING * 4,
                TEXT_HEIGHT,
                COLOR_BACKGROUND);
        }

        lcd.Set_Text_Mode(1);
        lcd.Set_Text_Back_colour(COLOR_BACKGROUND);
        uint16_t textColor = m_percentProgress == 100 ? COLOR_GREEN
                           : m_state == State::Active ? COLOR_CONTENT_EMPHASIZED
                           : COLOR_CONTENT_PRIMARY;
        lcd.Set_Text_colour(textColor);
        lcd.Set_Text_Size(1);
        const uint16_t strWidth = strlen(m_name) * (TEXT_WIDTH + 1) - 1; // +1 space between letters, -1 last space
        const uint16_t maxWidth = m_width - BORDER_WIDTH * 2 - BORDER_PADDING * 4;
        const uint16_t strOffset = (maxWidth - strWidth) / 2;
        lcd.Print_String(
            m_name,
            m_x + BORDER_PADDING * 2 + BORDER_WIDTH + strOffset,
            m_y + m_height - BORDER_PADDING * 3 - BORDER_WIDTH - TEXT_HEIGHT);
    }

    m_drawFlags = DrawFlags::None;
}

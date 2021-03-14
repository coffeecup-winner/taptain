#include "Widget.h"

#include <string.h>

#include "Config.h"
#include "Colors.h"

constexpr uint16_t TEXT_WIDTH = 5;
constexpr uint16_t TEXT_HEIGHT = 7;

void Widget::SetName(const char* name)
{
    strncpy(m_name, name, MAX_TEXT_LEN);
}

void Widget::Draw(const LCDWIKI_KBV& lcd)
{
    constexpr uint16_t BORDER_PADDING = 3;
    constexpr uint16_t BORDER_WIDTH = 7;

    lcd.Fill_Rect(
        m_x + BORDER_PADDING,
        m_y + BORDER_PADDING,
        m_width - BORDER_PADDING * 2,
        BORDER_WIDTH,
        COLOR_BLUE);
    lcd.Fill_Rect(
        m_x + BORDER_PADDING,
        m_y + m_height - BORDER_PADDING - BORDER_WIDTH,
        m_width - BORDER_PADDING * 2,
        BORDER_WIDTH,
        COLOR_BLUE);
    lcd.Fill_Rect(
        m_x + BORDER_PADDING,
        m_y + BORDER_PADDING + BORDER_WIDTH,
        BORDER_WIDTH,
        m_height - BORDER_PADDING * 2 - BORDER_WIDTH * 2,
        COLOR_BLUE);
    lcd.Fill_Rect(
        m_x + m_width - BORDER_PADDING - BORDER_WIDTH,
        m_y + BORDER_PADDING + BORDER_WIDTH,
        BORDER_WIDTH,
        m_height - BORDER_PADDING * 2 - BORDER_WIDTH * 2,
        COLOR_BLUE);

    lcd.Set_Text_Mode(1);
    lcd.Set_Text_Back_colour(COLOR_BACKGROUND);
    lcd.Set_Text_colour(COLOR_CONTENT_PRIMARY);
    lcd.Set_Text_Size(1);
    const uint16_t strWidth = strlen(m_name) * (TEXT_WIDTH + 1) - 1; // +1 space between letters, -1 last space
    const uint16_t maxWidth = m_width - BORDER_WIDTH * 2 - BORDER_PADDING * 4;
    const uint16_t strOffset = (maxWidth - strWidth) / 2;
    lcd.Print_String(
        m_name,
        m_x + BORDER_PADDING * 2 + BORDER_WIDTH + strOffset,
        m_y + m_height - BORDER_PADDING * 2 - BORDER_WIDTH - TEXT_HEIGHT);
}

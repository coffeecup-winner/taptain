#include "Widget.h"

#include <string.h>

#define COLOR_PATTERN_SOLARIZED_DARK
#include "Colors.h"

constexpr uint16_t TEXT_WIDTH = 5;
constexpr uint16_t TEXT_HEIGHT = 7;

void Widget::SetName(const char* name)
{
    strncpy(m_name, name, MAX_TEXT_LEN);
}

void Widget::Draw(const LCDWIKI_KBV& lcd, const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h)
{
    constexpr uint16_t BORDER_PADDING = 3;
    constexpr uint16_t BORDER_WIDTH = 7;

    lcd.Fill_Rect(
        x + BORDER_PADDING,
        y + BORDER_PADDING,
        w - BORDER_PADDING * 2,
        BORDER_WIDTH,
        COLOR_BLUE);
    lcd.Fill_Rect(
        x + BORDER_PADDING,
        y + h - BORDER_PADDING - BORDER_WIDTH,
        w - BORDER_PADDING * 2,
        BORDER_WIDTH,
        COLOR_BLUE);
    lcd.Fill_Rect(
        x + BORDER_PADDING,
        y + BORDER_PADDING + BORDER_WIDTH,
        BORDER_WIDTH,
        h - BORDER_PADDING * 2 - BORDER_WIDTH * 2,
        COLOR_BLUE);
    lcd.Fill_Rect(
        x + w - BORDER_PADDING - BORDER_WIDTH,
        y + BORDER_PADDING + BORDER_WIDTH,
        BORDER_WIDTH,
        h - BORDER_PADDING * 2 - BORDER_WIDTH * 2,
        COLOR_BLUE);

    lcd.Set_Text_Mode(1);
    lcd.Set_Text_Back_colour(COLOR_BACKGROUND);
    lcd.Set_Text_colour(COLOR_CONTENT_PRIMARY);
    lcd.Set_Text_Size(1);
    const uint16_t strWidth = strlen(m_name) * (TEXT_WIDTH + 1) - 1; // +1 space between letters, -1 last space
    const uint16_t maxWidth = w - BORDER_WIDTH * 2 - BORDER_PADDING * 4;
    const uint16_t strOffset = (maxWidth - strWidth) / 2;
    lcd.Print_String(
        m_name,
        x + BORDER_PADDING * 2 + BORDER_WIDTH + strOffset,
        y + h - BORDER_PADDING * 2 - BORDER_WIDTH - TEXT_HEIGHT);
}

#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>
#include <TouchScreen.h>

#include "Config.h"
#include "Colors.h"
#include "CommandBuffer.h"
#include "Error.h"
#include "Touch.h"
#include "Widget.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

LCDWIKI_KBV g_lcd(ILI9486, LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Touch g_touch(g_lcd.Get_Display_Width(), g_lcd.Get_Display_Height());

constexpr uint8_t WIDGETS_COUNT = 6;
Widget g_widgets[WIDGETS_COUNT] = {
    Widget(0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3),
    Widget(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3),
    Widget(0, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3),
    Widget(SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3),
    Widget(0, 2 * SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3),
    Widget(SCREEN_WIDTH / 2, 2 * SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3),
};
CommandBuffer<128> g_cmdBuffer;

void setup()
{
    g_lcd.Init_LCD();
    g_lcd.Fill_Screen(COLOR_BACKGROUND);

    g_widgets[0].SetName("01234567890123456789");
    g_widgets[1].SetName("TEST WIDGET");
    g_widgets[2].SetName("MORE TESTS");
    g_widgets[3].SetName("RUN A PROCESS");
    g_widgets[4].SetName("BREW SOME TEA");
    g_widgets[5].SetName("LAUNCH THE SATELLITE");

    for (uint8_t i = 0; i < WIDGETS_COUNT; i++) {
        g_widgets[i].Draw(g_lcd);
    }

    Serial.begin(57600);
    while (!Serial);

    g_lcd.Set_Draw_color(COLOR_GREEN);
}

void serialEvent()
{
    while (Serial.available()) {
        g_cmdBuffer.Put((uint8_t)Serial.read());
    }
}

void loop()
{
    // I/O commands
    uint8_t x;
    if (g_cmdBuffer.GetData(&x)) {
        uint8_t y;
        assert(g_cmdBuffer.GetData(&y));
        g_lcd.Draw_Pixel(x, y);
    }

    // User input
    const uint16_t iTouch = g_touch.GetTouchIndex([](const uint16_t x, const uint16_t y) {
        const uint8_t iX = x / (SCREEN_WIDTH / 2);
        const uint8_t iY = y / (SCREEN_HEIGHT / 3);
        return (2 - iY) * 2 + iX;
    });

    if (iTouch != Touch::INVALID_INDEX) {
        g_widgets[iTouch].Tap();
    }

    // Redrawing
    for (uint8_t i = 0; i < WIDGETS_COUNT; i++) {
        g_widgets[i].Draw(g_lcd);
    }
}

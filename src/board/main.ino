#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>
#include <TouchScreen.h>

#include "Config.h"
#include "Colors.h"
#include "CommandBuffer.h"
#include "Error.h"
#include "Widget.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

LCDWIKI_KBV g_lcd(ILI9486, LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen g_touchscreen(TS_XP, TS_YP, TS_XM, TS_YM, TS_RESISTANCE);

Widget widget[6];

void setup()
{
    g_lcd.Init_LCD();
    g_lcd.Fill_Screen(COLOR_BACKGROUND);

    widget[0].SetName("01234567890123456789");
    widget[0].Draw(g_lcd, 0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);

    widget[1].SetName("TEST WIDGET");
    widget[1].Draw(g_lcd, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);

    widget[2].SetName("MORE TESTS");
    widget[2].Draw(g_lcd, 0, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);

    widget[3].SetName("RUN A PROCESS");
    widget[3].Draw(g_lcd, SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);

    widget[4].SetName("BREW SOME TEA");
    widget[4].Draw(g_lcd, 0, 2 * SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);

    widget[5].SetName("LAUNCH THE SATELLITE");
    widget[5].Draw(g_lcd, SCREEN_WIDTH / 2, 2 * SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);

    Serial.begin(57600);
    while (!Serial);

    g_lcd.Set_Draw_color(COLOR_GREEN);
}

CommandBuffer<128> cmdBuffer;

void serialEvent()
{
    while (Serial.available()) {
        cmdBuffer.Put((uint8_t)Serial.read());
    }
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{
    uint8_t x;
    if (cmdBuffer.GetData(&x))
    {
        uint8_t y;
        assert(cmdBuffer.GetData(&y));
        g_lcd.Draw_Pixel(x, y);
    }
    const TSPoint p = g_touchscreen.getPoint();
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    {
        const uint16_t x = map(p.x, TS_XLO, TS_XHI, 0, g_lcd.Get_Display_Width());
        const uint16_t y = map(p.y, TS_YLO, TS_YHI, 0, g_lcd.Get_Display_Height());
        Serial.print("X: ");
        Serial.println(x, DEC);
        Serial.print("Y: ");
        Serial.println(y, DEC);
    }
}

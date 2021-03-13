#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>

#include "CommandBuffer.h"
#include "Error.h"

// For Arduino Uno
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

LCDWIKI_KBV g_lcd(ILI9486, LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup() 
{
    g_lcd.Init_LCD();
    g_lcd.Fill_Screen(0x0000);
    g_lcd.Set_Text_Mode(0);

    g_lcd.Set_Text_Back_colour(0x0000);
    g_lcd.Set_Text_colour(0x07E0);
    g_lcd.Set_Text_Size(3);
    g_lcd.Print_String("Hello, World!", 0, 0);

    g_lcd.Fill_Round_Rectangle(0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 5);
    g_lcd.Set_Draw_color(0x0000);
    g_lcd.Fill_Round_Rectangle(10, 10, SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 10, 5);

    Serial.begin(57600);
    while (!Serial);

    g_lcd.Set_Draw_color(0xffff);
}

CommandBuffer<128> cmdBuffer;

void serialEvent()
{
    while (Serial.available()) {
        cmdBuffer.Put((uint8_t)Serial.read());
    }
}

void loop()
{
    uint8_t x;
    if (cmdBuffer.GetData(&x))
    {
        uint8_t y;
        assert(cmdBuffer.GetData(&y));
        g_lcd.Draw_Pixel(x, y);
    }
}

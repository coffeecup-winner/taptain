#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>

// For Arduino Uno
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

LCDWIKI_KBV g_lcd(ILI9486, LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup() 
{
    g_lcd.Init_LCD();
    g_lcd.Fill_Screen(0x0000);
    g_lcd.Set_Text_Mode(0);
}

void loop() 
{
    g_lcd.Fill_Screen(0x0000);
    g_lcd.Set_Text_Back_colour(0x0000);
    g_lcd.Set_Text_colour(0x07E0);
    g_lcd.Set_Text_Size(3);
    g_lcd.Print_String("Hello, World!", 0, 0);

    delay(5000);
}

#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>
#include <TouchScreen.h>

#include <string.h>

#include "Config.h"
#include "Colors.h"
#include "CommandBuffer.h"
#include "Error.h"
#include "Request.h"
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

    for (uint8_t i = 0; i < WIDGETS_COUNT; i++) {
        g_widgets[i].Draw(g_lcd);
    }

    Serial.begin(57600);
    while (!Serial);
}

uint8_t g_bIsInBatch = false;

void loop()
{
    // Reading command data
    while (Serial.available()) {
        g_cmdBuffer.Put((uint8_t)Serial.read());
    }

    // Processing commands
    Command cmd;
    while (g_cmdBuffer.GetCommand(&cmd)) {
        switch (cmd.type) {
            case Command::Type::Init:
                g_widgets[cmd.init.iWidget].SetName(cmd.init.name);
                g_widgets[cmd.init.iWidget].Reset();
                break;
            case Command::Type::BeginBatch:
                g_bIsInBatch = true;
                break;
            case Command::Type::EndBatch:
                g_bIsInBatch = false;
                break;
            default:
                Error("Unhandled command");
                break;
        }
        
        if (!g_bIsInBatch) {
            Serial.write("OK");
        }
    }

    if (g_bIsInBatch) {
        return;
    }

    // User input
    const uint16_t iTouch = g_touch.GetTouchIndex([](const uint16_t x, const uint16_t y) {
        const uint8_t iX = x / (SCREEN_WIDTH / 2);
        const uint8_t iY = y / (SCREEN_HEIGHT / 3);
        return (2 - iY) * 2 + iX;
    });

    if (iTouch != Touch::INVALID_INDEX) {
        Request request;
        memset(&request, 0, sizeof(request));
        g_widgets[iTouch].Tap(&request);
        if (request.type != Request::Type::None) {
            request.iWidget = iTouch;
            Serial.write((const uint8_t*)&request, sizeof(request));
        }
    }

    // Redrawing
    for (uint8_t i = 0; i < WIDGETS_COUNT; i++) {
        g_widgets[i].Draw(g_lcd);
    }
}

#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>
#include <TouchScreen.h>

#include <string.h>

#include "Config.h"
#include "Colors.h"
#include "CommandBuffer.h"
#include "Error.h"
#include "Message.h"
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
            case Command::Type::Progress:
                g_widgets[cmd.progress.iWidget].SetProgress(cmd.progress.percent);
                break;
            default:
                Error("Unhandled command");
                break;
        }

        if (!g_bIsInBatch) {
            Message message = {};
            message.type = Message::Type::OK;
            Serial.write((const uint8_t*)&message, sizeof(message));
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
        uint8_t bOutsideTapRegistered = false;
        for (uint8_t i = 0; i < WIDGETS_COUNT; i++) {
            if (i != iTouch) {
                Message message = {};
                g_widgets[i].TapElsewhere(&message);
                if (message.type != Message::Type::None) {
                    message.iWidget = i;
                    Serial.write((const uint8_t*)&message, sizeof(message));
                    bOutsideTapRegistered = true;
                }
            }
        }
        if (!bOutsideTapRegistered) {
            Message message = {};
            g_widgets[iTouch].Tap(&message);
            if (message.type != Message::Type::None) {
                message.iWidget = iTouch;
                Serial.write((const uint8_t*)&message, sizeof(message));
            }
        }
    }

    // Redrawing
    for (uint8_t i = 0; i < WIDGETS_COUNT; i++) {
        g_widgets[i].Draw(g_lcd);
    }
}

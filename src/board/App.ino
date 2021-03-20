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

constexpr uint8_t WIDGETS_MAX_COUNT = 24;
Widget g_widgets[WIDGETS_MAX_COUNT];
struct {
    uint8_t width: 2; // max: 4
    uint8_t height: 3; // max: 6
    uint8_t count: 5; // max: 24
} g_widgetConfig;
CommandBuffer<128> g_cmdBuffer;

void setup()
{
    g_lcd.Init_LCD();
    g_lcd.Fill_Screen(COLOR_BACKGROUND);

    Serial.begin(57600);
    while (!Serial);
}

struct {
    bool bIsInBatch : 1;
    bool bClearScreen : 1;
} g_flags;

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
                if (cmd.init.iWidget < g_widgetConfig.count) {
                    g_widgets[cmd.init.iWidget].SetName(cmd.init.name);
                    g_widgets[cmd.init.iWidget].Reset();
                }
                break;
            case Command::Type::BeginBatch:
                g_flags.bIsInBatch = true;
                break;
            case Command::Type::EndBatch:
                g_flags.bIsInBatch = false;
                break;
            case Command::Type::Progress:
                if (cmd.progress.iWidget < g_widgetConfig.count) {
                    g_widgets[cmd.progress.iWidget].SetProgress(cmd.progress.percent);
                }
                break;
            case Command::Type::Configure:
                assert(cmd.configure.width <= 4);
                assert(cmd.configure.height <= 6);
                uint8_t i = 0;
                const uint8_t widgetWidth = SCREEN_WIDTH / cmd.configure.width;
                const uint8_t widgetHeight = SCREEN_HEIGHT / cmd.configure.height;
                for (uint16_t y = 0; (y + widgetHeight) <= SCREEN_HEIGHT; y += widgetHeight) {
                    for (uint16_t x = 0; (x + widgetWidth) <= SCREEN_WIDTH; x += widgetWidth) {
                        g_widgets[i] = Widget(x, y, widgetWidth, widgetHeight);
                        i++;
                    }
                }
                while (i < g_widgetConfig.count) {
                    g_widgets[i] = Widget();
                    i++;
                }
                g_widgetConfig.width = cmd.configure.width;
                g_widgetConfig.height = cmd.configure.height;
                g_widgetConfig.count = cmd.configure.width * cmd.configure.height;
                g_flags.bClearScreen = true;
                break;
            default:
                Error("Unhandled command");
                break;
        }

        if (!g_flags.bIsInBatch) {
            Message message = {};
            message.type = Message::Type::OK;
            Serial.write((const uint8_t*)&message, sizeof(message));
        }
    }

    if (g_flags.bIsInBatch) {
        return;
    }

    // User input
    const uint16_t iTouch = g_touch.GetTouchIndex([&](const uint16_t x, const uint16_t y) {
        const uint8_t iX = x / (SCREEN_WIDTH / g_widgetConfig.width);
        const uint8_t iY = y / (SCREEN_HEIGHT / g_widgetConfig.height);
        return (g_widgetConfig.height - iY - 1) * g_widgetConfig.width + iX;
    });

    if (iTouch != Touch::INVALID_INDEX) {
        uint8_t bOutsideTapRegistered = false;
        for (uint8_t i = 0; i < g_widgetConfig.count; i++) {
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
    if (g_flags.bClearScreen) {
        // Alternatively, erase existing instead of clearing the screen?
        g_lcd.Fill_Screen(COLOR_BACKGROUND);
        g_flags.bClearScreen = false;
    }
    for (uint8_t i = 0; i < g_widgetConfig.count; i++) {
        g_widgets[i].Draw(g_lcd);
    }
}

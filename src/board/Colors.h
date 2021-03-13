#pragma once

#include <stdint.h>

constexpr uint16_t RgbToRgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
}

// Solarized color pattern (https://ethanschoonover.com/solarized)
constexpr uint16_t COLOR_SOLARIZED_BASE03 =   RgbToRgb565(0x00, 0x2b, 0x36);
constexpr uint16_t COLOR_SOLARIZED_BASE02 =   RgbToRgb565(0x07, 0x36, 0x42);
constexpr uint16_t COLOR_SOLARIZED_BASE01 =   RgbToRgb565(0x58, 0x6e, 0x75);
constexpr uint16_t COLOR_SOLARIZED_BASE00 =   RgbToRgb565(0x65, 0x7b, 0x83);
constexpr uint16_t COLOR_SOLARIZED_BASE0 =    RgbToRgb565(0x83, 0x94, 0x96);
constexpr uint16_t COLOR_SOLARIZED_BASE1 =    RgbToRgb565(0x93, 0xa1, 0xa1);
constexpr uint16_t COLOR_SOLARIZED_BASE2 =    RgbToRgb565(0xee, 0xe8, 0xd5);
constexpr uint16_t COLOR_SOLARIZED_BASE3 =    RgbToRgb565(0xfd, 0xf6, 0xe3);
constexpr uint16_t COLOR_SOLARIZED_YELLOW =   RgbToRgb565(0xb5, 0x89, 0x00);
constexpr uint16_t COLOR_SOLARIZED_ORANGE =   RgbToRgb565(0xcb, 0x4b, 0x16);
constexpr uint16_t COLOR_SOLARIZED_RED =      RgbToRgb565(0xdc, 0x32, 0x2f);
constexpr uint16_t COLOR_SOLARIZED_MAGENTA =  RgbToRgb565(0xd3, 0x36, 0x82);
constexpr uint16_t COLOR_SOLARIZED_VIOLET =   RgbToRgb565(0x6c, 0x71, 0xc4);
constexpr uint16_t COLOR_SOLARIZED_BLUE =     RgbToRgb565(0x26, 0x8b, 0xd2);
constexpr uint16_t COLOR_SOLARIZED_CYAN =     RgbToRgb565(0x2a, 0xa1, 0x98);
constexpr uint16_t COLOR_SOLARIZED_GREEN =    RgbToRgb565(0x85, 0x99, 0x00);

#if defined(COLOR_PATTERN_SOLARIZED_DARK)
    constexpr uint16_t COLOR_BACKGROUND =           COLOR_SOLARIZED_BASE03;
    constexpr uint16_t COLOR_BACKGROUND_HIGHLIGHT = COLOR_SOLARIZED_BASE02;
    constexpr uint16_t COLOR_CONTENT_PRIMARY =      COLOR_SOLARIZED_BASE0;
    constexpr uint16_t COLOR_CONTENT_SECONDARY =    COLOR_SOLARIZED_BASE01;
    constexpr uint16_t COLOR_CONTENT_EMPHASIZED =   COLOR_SOLARIZED_BASE1;
    constexpr uint16_t COLOR_YELLOW =               COLOR_SOLARIZED_YELLOW;
    constexpr uint16_t COLOR_ORANGE =               COLOR_SOLARIZED_ORANGE;
    constexpr uint16_t COLOR_RED =                  COLOR_SOLARIZED_RED;
    constexpr uint16_t COLOR_MAGENTA =              COLOR_SOLARIZED_MAGENTA;
    constexpr uint16_t COLOR_VIOLET =               COLOR_SOLARIZED_VIOLET;
    constexpr uint16_t COLOR_BLUE =                 COLOR_SOLARIZED_BLUE;
    constexpr uint16_t COLOR_CYAN =                 COLOR_SOLARIZED_CYAN;
    constexpr uint16_t COLOR_GREEN =                COLOR_SOLARIZED_GREEN;
#elif defined(COLOR_PATTERN_SOLARIZED_LIGHT)
    constexpr uint16_t COLOR_BACKGROUND =           COLOR_SOLARIZED_BASE3;
    constexpr uint16_t COLOR_BACKGROUND_HIGHLIGHT = COLOR_SOLARIZED_BASE2;
    constexpr uint16_t COLOR_CONTENT_PRIMARY =      COLOR_SOLARIZED_BASE00;
    constexpr uint16_t COLOR_CONTENT_SECONDARY =    COLOR_SOLARIZED_BASE1;
    constexpr uint16_t COLOR_CONTENT_EMPHASIZED =   COLOR_SOLARIZED_BASE01;
    constexpr uint16_t COLOR_YELLOW =               COLOR_SOLARIZED_YELLOW;
    constexpr uint16_t COLOR_ORANGE =               COLOR_SOLARIZED_ORANGE;
    constexpr uint16_t COLOR_RED =                  COLOR_SOLARIZED_RED;
    constexpr uint16_t COLOR_MAGENTA =              COLOR_SOLARIZED_MAGENTA;
    constexpr uint16_t COLOR_VIOLET =               COLOR_SOLARIZED_VIOLET;
    constexpr uint16_t COLOR_BLUE =                 COLOR_SOLARIZED_BLUE;
    constexpr uint16_t COLOR_CYAN =                 COLOR_SOLARIZED_CYAN;
    constexpr uint16_t COLOR_GREEN =                COLOR_SOLARIZED_GREEN;
#else
#error Color pattern not set
#endif

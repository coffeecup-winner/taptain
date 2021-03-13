#pragma once

#include <stdint.h>

#include <LCDWIKI_KBV.h>

struct IDrawable {
    virtual void Draw(const LCDWIKI_KBV& lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
};

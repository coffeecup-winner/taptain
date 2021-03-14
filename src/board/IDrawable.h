#pragma once

#include <stdint.h>

#include <LCDWIKI_KBV.h>

class IDrawable {
public:
    virtual void Draw(const LCDWIKI_KBV& lcd, const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h) = 0;
};

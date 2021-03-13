#pragma once

#include "IDrawable.h"

class Widget : public IDrawable {
    static constexpr uint16_t MAX_TEXT_LEN = 21;

    char m_name[MAX_TEXT_LEN];

public:
    void Draw(const LCDWIKI_KBV& lcd, const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h) override;

    void SetName(const char* name);
};

#pragma once

#include <Arduino.h>
#include <TouchScreen.h>

#include "Config.h"

class Touch {
    // Touchscreen sensivity
    constexpr static uint16_t TS_MINPRESSURE = 10;
    constexpr static uint16_t TS_MAXPRESSURE = 1000;
    constexpr static unsigned long TS_TIME_DELAY_MS = 30;

    TouchScreen m_touchscreen;
    const uint16_t m_width;
    const uint16_t m_height;
    uint16_t m_firstTouchIndex;
    uint16_t m_lastTouchIndex;
    unsigned long m_lastTouchMillis;

public:
    constexpr static uint16_t INVALID_INDEX = 0xffff;

    Touch(const uint16_t width, const uint16_t height)
        : m_touchscreen(TS_XP, TS_YP, TS_XM, TS_YM, TS_RESISTANCE)
        , m_width(width)
        , m_height(height)
        , m_firstTouchIndex(INVALID_INDEX)
        , m_lastTouchIndex(INVALID_INDEX)
        , m_lastTouchMillis(0)
    { }

    // Return a user-mapped touch index
    // The index is only returned if it the first and last touch events are the same,
    // and also a time delay had passed after the last touch event.
    // Essentially, this is the typical "Click" event.
    template <typename TFunc>
    uint16_t GetTouchIndex(TFunc transform) {
        const TSPoint p = m_touchscreen.getPoint();
        if (p.z > TS_MINPRESSURE && p.z < TS_MAXPRESSURE) {
            m_lastTouchMillis = millis();
            const uint16_t mappedX = map(p.x, TS_XLO, TS_XHI, 0, m_width);
            const uint16_t mappedY = map(p.y, TS_YLO, TS_YHI, 0, m_height);
            if (m_firstTouchIndex == INVALID_INDEX) {
                m_lastTouchIndex = m_firstTouchIndex = transform(mappedX, mappedY);
            } else {
                m_lastTouchIndex = transform(mappedX, mappedY);
            }
            return INVALID_INDEX;
        } else {
            if ((millis() - m_lastTouchMillis) > TS_TIME_DELAY_MS) {
                if (m_firstTouchIndex == m_lastTouchIndex) {
                    const uint16_t index = m_lastTouchIndex;
                    m_lastTouchIndex = m_firstTouchIndex = INVALID_INDEX;
                    return index;
                }
                m_lastTouchIndex = m_firstTouchIndex = INVALID_INDEX;
                return INVALID_INDEX;
            }
        }
    }
};

#pragma once

#include <stdint.h>

#include "Error.h"

template <int Size>
class CircularBuffer {
    uint8_t m_cmd[Size];
    uint8_t m_iStart = 0;
    uint8_t m_iTemp = 0;
    uint8_t m_iEnd = 0;

public:
    void Put(const uint8_t data)
    {
        m_cmd[m_iTemp] = data;
        m_iTemp = (m_iTemp + 1) % Size;
        if (m_iTemp == m_iStart) {
            Error("Circular buffer overflow");
        }
    }

    void Commit()
    {
        m_iEnd = m_iTemp;
    }

    bool HasData() const
    {
        return m_iStart != m_iEnd;
    }

    bool GetData(uint8_t* pData)
    {
        if (!HasData()) {
            return false;
        }
        *pData = m_cmd[m_iStart];
        m_iStart = (m_iStart + 1) % Size;
        return true;
    }
};

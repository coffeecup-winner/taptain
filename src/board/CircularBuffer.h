#pragma once

#include <stdint.h>

#include "Error.h"

template <int Size>
class CircularBuffer {
    uint8_t cmd[Size];
    uint8_t iStart = 0;
    uint8_t iTemp = 0;
    uint8_t iEnd = 0;

public:
    void Put(const uint8_t data)
    {
        cmd[iTemp] = data;
        iTemp = (iTemp + 1) % Size;
        if (iTemp == iStart) {
            Error("Circular buffer overflow");
        }
    }

    void Commit()
    {
        iEnd = iTemp;
    }

    bool HasData() const
    {
        return iStart != iEnd;
    }

    bool GetData(uint8_t* data)
    {
        if (!HasData()) {
            return false;
        }
        *data = cmd[iStart];
        iStart = (iStart + 1) % Size;
        return true;
    }
};

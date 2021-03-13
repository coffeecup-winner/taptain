#pragma once

#include "CircularBuffer.h"

// Command buffer allows receiving arbitrary byte packets from the host,
// decoding them as follows:
// 0x00 - end of packet
// 0xff - escape byte:
//   * 0x00 - 0x00
//   * 0xff - 0xff
//   * 0x01..0xfe - invalid packet
// 0x01..0xfe - 0x01..0xfe

template <int Size>
class CommandBuffer {
    CircularBuffer<Size> buffer;
    uint8_t bEscape;

public:
    void Put(const uint8_t data)
    {
        switch (data) {
            case 0x00:
                if (bEscape) {
                    buffer.Put(data);
                    bEscape = false;
                } else {
                    buffer.Commit();
                }
                break;
            case 0xff:
                if (bEscape) {
                    buffer.Put(data);
                    bEscape = false;
                } else {
                    bEscape = true;
                }
                break;
            default:
                if (bEscape) {
                    Error("Invalid packet");
                }
                buffer.Put(data);
                break;
        }
    }

    bool HasData() const
    {
        return buffer.HasData();
    }

    bool GetData(uint8_t* data)
    {
        return buffer.GetData(data);
    }
};

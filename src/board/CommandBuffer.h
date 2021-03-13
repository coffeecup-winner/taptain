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
    CircularBuffer<Size> m_buffer;
    uint8_t m_bEscape;

public:
    void Put(const uint8_t data)
    {
        switch (data) {
            case 0x00:
                if (m_bEscape) {
                    m_buffer.Put(data);
                    m_bEscape = false;
                } else {
                    m_buffer.Commit();
                }
                break;
            case 0xff:
                if (m_bEscape) {
                    m_buffer.Put(data);
                    m_bEscape = false;
                } else {
                    m_bEscape = true;
                }
                break;
            default:
                if (m_bEscape) {
                    Error("Invalid packet");
                }
                m_buffer.Put(data);
                break;
        }
    }

    bool HasData() const
    {
        return m_buffer.HasData();
    }

    bool GetData(uint8_t* pData)
    {
        return m_buffer.GetData(pData);
    }
};

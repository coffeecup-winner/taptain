#pragma once

#include "CircularBuffer.h"

// The command buffer allows receiving command packets from the host,
// decoding data as follows and then parsing the commands:
// 0x00 - end of packet
// 0xff - escape byte:
//   * 0x00 - 0x00
//   * 0xff - 0xff
//   * 0x01..0xfe - invalid packet
// 0x01..0xfe - 0x01..0xfe

struct Command {
    enum class Type : uint8_t {
        Init = 0,
        BeginBatch,
        EndBatch,
        Progress,
    } type;
    union {
        struct {
            uint8_t iWidget;
            char name[21];
        } init;
        struct {
            uint8_t iWidget;
            uint8_t percent;
        } progress;
    };
};

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

    bool GetCommand(Command* pCommand)
    {
        Command::Type type;
        if (!m_buffer.GetData((uint8_t*)&type)) {
            return false;
        }
        pCommand->type = type;
        switch (type) {
            case Command::Type::Init:
                assert(m_buffer.GetData(&pCommand->init.iWidget));
                for (uint8_t i = 0; i < sizeof(Command::init.name); i++) {
                    assert(m_buffer.GetData(&pCommand->init.name[i]));
                }
                return true;
            case Command::Type::BeginBatch:
            case Command::Type::EndBatch:
                return true;
            case Command::Type::Progress:
                assert(m_buffer.GetData(&pCommand->progress.iWidget));
                assert(m_buffer.GetData(&pCommand->progress.percent));
                return true;
            default:
                Error("Invalid command type received from host");
                return false;
        }
    }
};

#pragma once
#include <cstdint>
#include <vector>
#include <cstddef>

/*
 * Frame Utility - Helper functions for frame construction (for UART protocol design) and parsing.
 *
 * FRAME_FORMAT_1: [START_WORD (2 bytes)] + [LEN (1 byte)] + [TYPE (1 byte)] + [PAYLOAD (LEN bytes)] + [CRC16 (2 bytes little-endian)]
 *  - START_WORD: 0xAA55
 *
 */

namespace uart_protocol
{
    struct Frame
    {
        uint8_t type = 0;
        std::vector<uint8_t> payload;
    };


    /* CRC-16-CCITT Implementation 
        * Polynomial: 0x1021
        * Initial value: 0xFFFF
        * No reflection, no final XOR
    https://gist.github.com/rafacouto/59326c90d6a55f86a3ba
    https://os.mbed.com/users/hudakz/code/CRC16_CCITT/file/6ecc3a64bf7b/CRC16_CCITT.cpp/
    */
    inline uint16_t crc16_ccitt(const uint8_t *data, size_t len)
    {
        uint16_t crc = 0xFFFF;
        for (size_t i = 0; i < len; ++i)
        {
            crc ^= static_cast<uint16_t>(data[i]) << 8;
            for (int j = 0; j < 8; ++j)
            {
                if (crc & 0x8000)
                    crc = (crc << 1) ^ 0x1021;
                else
                    crc <<= 1;
            }
        }
        return crc;
    }

}
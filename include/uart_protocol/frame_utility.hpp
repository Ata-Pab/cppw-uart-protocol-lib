#pragma once
#include <cstdint>
#include <vector>
#include <cstddef>

/*
 * Frame Utility - Helper functions for frame construction (for UART protocol design) and parsing.
 *
 * FRAME_FORMAT: [START_WORD (2 bytes)] + [LEN (1 byte)] + [TYPE (1 byte)] + [PAYLOAD (LEN bytes)] + [CRC16 (2 bytes little-endian)]
 *  - START_WORD: 0xAA55
 *
 */

namespace uart_protocol
{
    struct Frame
    {
        static constexpr uint16_t START_WORD = 0xAA55; // Frame start identifier - Edit if needed
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

    inline std::vector<uint8_t> construct_frame(const Frame &frame)
    {
        std::vector<uint8_t> raw_frame; // buffer for the constructed frame

        // Reserve space to avoid multiple allocations
        // [START_WORD (2 bytes)] + [LEN (1 byte)] + [TYPE (1 byte)] + [PAYLOAD (LEN bytes)] + [CRC16 (2 bytes little-endian)]
        raw_frame.reserve(2 + 1 + 1 + frame.payload.size() + 2);

        // START_WORD
        raw_frame.push_back(static_cast<uint8_t>(Frame::START_WORD & 0xFF)); // compile-time cast
        raw_frame.push_back(static_cast<uint8_t>((Frame::START_WORD >> 8) & 0xFF));

        // LEN
        uint8_t len = static_cast<uint8_t>(frame.payload.size());
        raw_frame.push_back(len);

        // TYPE
        raw_frame.push_back(frame.type);

        // PAYLOAD
        raw_frame.insert(raw_frame.end(), frame.payload.begin(), frame.payload.end()); // Insert a range into the %vector.

        // CRC16
        uint16_t crc = crc16_ccitt(raw_frame.data(), raw_frame.size()); // Calculate CRC16 over the entire frame except the CRC itself
        raw_frame.push_back(static_cast<uint8_t>(crc & 0xFF));
        raw_frame.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));

        return raw_frame;
    }
    

}
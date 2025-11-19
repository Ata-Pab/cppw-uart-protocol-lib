#pragma once
#include <cstdint>
#include <cstddef>

namespace uart_protocol::config
{
    // Protocol frame start word
    inline constexpr uint16_t START_WORD = 0xAA55; // Frame start identifier - edit if needed

    // Protocol message types
    inline constexpr uint8_t START_WORD_TYPE = 0x01; // Frame type for START_WORD – edit if needed
    inline constexpr uint8_t ACK_TYPE = 0x02;        // Frame type for ACK – edit if needed

    // Max payload size
    inline constexpr size_t MAX_PAYLOAD_SIZE = 255; // Max payload size due to LEN being 1 byte

    // Default timeouts
    inline constexpr uint32_t DEFAULT_ACK_TIMEOUT_MS = 200; // Default timeout for ACK wait
} // namespace uart_protocol::config
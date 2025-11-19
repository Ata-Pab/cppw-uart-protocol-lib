#pragma once
#include <cstdint>
#include <cstddef>

/*
 * Protocol Configuration - Configuration constants for UART Protocol.
 * This header defines protocol-specific constants such as frame types,
 * start words, maximum payload sizes, and default timeouts.
 *
 * These configurations can be adjusted as needed for different applications.
 */

// Define the platform for timing utility, see timing_utility.hpp for other options
#ifndef USE_STD_CHRONO
    #define USE_STD_CHRONO 
#endif

// Namespace for protocol configuration constants
namespace uart_protocol::config
{
    // Protocol frame start word
    inline constexpr uint16_t START_WORD = 0xAA55; // Frame start identifier - edit if needed

    // Protocol message types
    inline constexpr uint8_t START_WORD_TYPE = 0x01;    // Frame type for START_WORD – edit if needed
    inline constexpr uint8_t ACK_TYPE = 0x02;           // Frame type for ACK – edit if needed
    inline constexpr uint8_t NACK_TYPE = 0x03;          // Frame type for NACK – edit if needed
    inline constexpr uint8_t DATA_TYPE = 0x04;          // Frame type for DATA – edit if needed
    inline constexpr uint8_t ARE_YOU_THERE_TYPE = 0x05; // Frame type for ARE_YOU_THERE (Keep-alive) – edit if needed
    inline constexpr uint8_t CMD_TYPE = 0x06;           // Frame type for CMD (Command frame) – edit if needed
    inline constexpr uint8_t RESP_TYPE = 0x07;          // Frame type for RESP (Response frame) – edit if needed
    inline constexpr uint8_t ERROR_TYPE = 0x08;         // Frame type for ERROR – edit if needed

    // Max payload size
    inline constexpr size_t MAX_PAYLOAD_SIZE = 255; // Max payload size due to LEN being 1 byte

    // Default timeouts
    inline constexpr uint32_t DEFAULT_ACK_TIMEOUT_MS = 200; // Default timeout for ACK wait
} // namespace uart_protocol::config
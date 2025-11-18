#pragma once
#include <cstdint> // uint8_t, uint32_t...
#include <cstddef> // size_t
#include <vector>

/*
 * Uart - Transport abstraction for the library.
 * Peripheral interface for UART communication.
 * This interface defines methods for sending and receiving data over UART.
 *
 * Implement this on each platform (STM32 HAL, ESP-IDF, POSIX/Boost.Asio, etc.)
 *
 * The functions are intentionally minimal:
 *  - init()/deinit() -> platform init/teardown
 *  - send_data(...)  -> synchronous send of bytes (blocking until bytes handed to driver)
 *  - receive_data(...) -> read available bytes into buffer (non-blocking recommended)
 *
 * Implementations can use interrupts/DMA internally but expose this minimal, testable API.
 */

namespace uart_protocol
{
    class Uart
    {
    public:
        virtual ~Uart() = default;

        // Initialize HW/driver. Return true on success.
        virtual bool init() = 0;

        // Deinitialize HW/driver, free resources.
        virtual void deinit() = 0;

        // Send data/bytes over UART. Blocking until all bytes are handed to driver.
        // Returns true if send was accepted (not necessarily physically transmitted yet).
        virtual bool send_data(const uint8_t *data, size_t size) = 0;

        // Receive available data/bytes from UART into buffer.
        // Read up to `max_bytes` and append into out. Return number of bytes read.
        // Non-blocking: return 0 if no data available.
        virtual size_t receive_data(uint8_t *out_buffer, size_t max_bytes) = 0;
    };
} // namespace uart_protocol
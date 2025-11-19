#pragma once
#include "peripheral.hpp"
#include "frame_utility.hpp"
#include <vector>
#include <chrono>
#include <thread>

/*
 * UART Protocol - Protocol layer implementation for UART communication.
 * This class provides methods to send and receive framed data over UART using the Uart interface.
 * It handles high-level operations: send_frame, wait_ack, send_start_word, etc.
 * This implementation is intentionally portable and blocking. For embedded you can
 * provide different wait mechanism (RTOS event, ISR, DMA callback).
 */
namespace uart_protocol
{
    class Protocol
    {
    private:
        uart_protocol::Uart &uart_;

    public:
        bool init()
        {
            return uart_.init();
        }
        void deinit()
        {
            uart_.deinit();
        }

        Protocol(uart_protocol::Uart &uart) : uart_(uart) {}

        // Send a framed data packet over UART.
        // Returns true if the frame was successfully sent.
        bool send_frame(uint8_t type, const std::vector<uint8_t> &payload)
        {
            Frame frame = {type, payload};
            auto raw_frame = construct_frame(frame);
            return uart_.send_data(raw_frame.data(), raw_frame.size());
        }
    };

} // namespace uart_protocol
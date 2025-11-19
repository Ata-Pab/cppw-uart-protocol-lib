#pragma once
#include "peripheral.hpp"
#include "protocol_config.hpp"
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

        // The `explicit` keyword is used to prevent implicit conversions and copy-initialization.
        // It ensures that the constructor cannot be called with a single argument implicitly,
        // which helps avoid unintentional conversions that might lead to bugs.
        explicit Protocol(uart_protocol::Uart &uart) : uart_(uart) {}

        // Send a framed data packet over UART.
        // Returns true if the frame was successfully sent.
        bool send_frame(uint8_t type, const std::vector<uint8_t> &payload)
        {
            Frame frame = {type, payload};
            auto raw_frame = construct_frame(frame);
            return uart_.send_data(raw_frame.data(), raw_frame.size());
        }

        /*
         * Send a framed data packet and wait for an ACK frame.
         * Returns true if the ACK was received within the timeout period.
         * @param type Frame type to send.
         * @param payload Payload data to send.
         * @param timeout_ms Timeout in milliseconds to wait for the ACK.
         * @return true if ACK received, false on timeout or error.
         */
        bool send_frame_wait_ack(uint8_t type, const std::vector<uint8_t> &payload, uint32_t timeout_ms = config::DEFAULT_ACK_TIMEOUT_MS)
        {
            // Send the frame first
            if (!send_frame(type, payload))
            {
                return false;
            }

            auto wait_until = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
            std::vector<uint8_t> recv_buffer;
            recv_buffer.reserve(config::MAX_PAYLOAD_SIZE); // Reserve buffer space to avoid multiple allocations

            uint8_t temp_buffer[64]; // Temporary buffer for receiving data

            // Wait for ACK frame
            while (std::chrono::steady_clock::now() < wait_until)
            {
                size_t n = uart_.receive_data(temp_buffer, sizeof(temp_buffer));

                if (n > 0)
                {
                    recv_buffer.insert(recv_buffer.end(), temp_buffer, temp_buffer + n);

                    // Try to parse frames from the received buffer
                    Frame received_frame;
                    while (parse_frame(recv_buffer, received_frame))
                    {
                        // Check if the received frame is an ACK
                        if (received_frame.type == config::ACK_TYPE)
                        {
                            return true; // ACK received
                        }
                    }
                }
                else
                {
                    // No data received, sleep briefly to avoid busy-waiting
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            return false; // Timeout waiting for ACK
        }

        // Send START_WORD over UART. No payload, just the start word.
        bool send_start_word()
        {
            return send_frame(config::START_WORD_TYPE, {});
        }

        // Send ACK frame over UART. No payload, just the ACK frame.
        bool send_ack()
        {
            return send_frame(config::ACK_TYPE, {});
        }
    };
} // namespace uart_protocol

#pragma once
#include "peripheral.hpp"
#include <deque>
#include <mutex>
#include <condition_variable>
#include <chrono>

/*
 * Win32 Uart Porting - UART peripheral implementation for Windows using Win32 API.
 *
 * This class implements the Uart interface defined in peripheral.hpp.
 * It provides methods to initialize, deinitialize, send, and receive data over a serial port on Windows.
 *
 * Note: This is a basic implementation and may require enhancements for production use.
 */

namespace uart_protocol
{
    class UartDemo : public Uart
    {
    private:
        bool initialized_{false};       // Modern default initialization C++11
        std::deque<uint8_t> rx_buffer_; // Preferred container for insertion and deletion methods at beginning and end
        std::deque<uint8_t> tx_buffer_; // Good performance for insertion and deletion at the front
        std::mutex mutex_;              // Mutex for thread-safe access to buffers
        std::condition_variable cv_;    // Condition variable for signaling data availability

    public:
        UartDemo() = default;
        ~UartDemo() override = default;

        bool init() override
        {
            initialized_ = true;
            return true;
        }
        void deinit() override { initialized_ = false; }

        // Send data/bytes over UART. Blocking until all bytes are handed to driver.
        bool send_data(const uint8_t *data, size_t size) override
        {
            if (!initialized_)
                return false;
            std::lock_guard<std::mutex> lock(mutex_);
            for (size_t i = 0; i < size; ++i)
            {
                tx_buffer_.push_back(data[i]);
            }
            cv_.notify_all(); // Notify any waiting receive_data calls
            return true;
        }

        // Receive available data/bytes from UART into buffer. Simulates bytes arriving to this UART
        size_t receive_data(uint8_t *out_buffer, size_t max_bytes) override
        {
            if (!initialized_)
                return 0;
            std::lock_guard<std::mutex> lock(mutex_);
            // cv_.wait_for(lock, std::chrono::milliseconds(100), [this]()
            //              { return !rx_buffer_.empty(); });

            size_t bytes_read = 0;
            while (bytes_read < max_bytes && !rx_buffer_.empty())
            {
                out_buffer[bytes_read++] = rx_buffer_.front();
                rx_buffer_.pop_front(); // Remove byte from buffer
            }
            return bytes_read;
        }

        // TEST FUNCTION: Simulate incoming data to the UARt. Push bytes into rx_buffer_ to simulate incoming data
        // Custom function for Win32 testing purposes - Embedded firmware would not have this
        void simulate_incoming_data(const std::vector<uint8_t> &bytes)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto mbyte : bytes)
            {
                rx_buffer_.push_back(mbyte);
            }
            cv_.notify_all(); // Notify any waiting receive_data calls
        }

        // TEST FUNCTION: Wait until at least `count` bytes are in the tx_buffer_ or timeout occurs
        bool wait_for_tx_size(size_t count, std::chrono::milliseconds timeout = std::chrono::milliseconds(100))
        {
            auto deadline = std::chrono::steady_clock::now() + timeout;
            std::unique_lock<std::mutex> uniquel(mutex_);
            return cv_.wait_until(uniquel, deadline, [&]
                                  { return tx_buffer_.size() >= count; });
        }
    };

} // namespace uart_protocol
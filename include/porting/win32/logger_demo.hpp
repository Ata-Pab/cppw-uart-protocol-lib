#pragma once
#include "uart_protocol/peripheral.hpp"
#include <windows.h>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

/*
 * Win32 Serial Port Logger - Real serial port implementation for Windows.
 *
 * This class implements the Uart interface to communicate with real serial ports (COM ports).
 * Designed to receive log data from ESP32 or other embedded devices via USB-to-Serial.
 *
 * Usage:
 *   LoggerDemo logger;
 *   logger.set_port("COM5");           // Set COM port
 *   logger.set_baudrate(115200);       // Set baud rate
 *   if (logger.init()) {
 *       // Port opened successfully
 *   }
 */

namespace uart_protocol
{
    class LoggerDemo : public Uart
    {
    private:
        std::thread read_thread_;
        std::atomic<bool> stop_reading_{false};
        std::vector<uint8_t> rx_buffer_;
        std::mutex rx_mutex_;

        /* USER CODE BEGIN */
        HANDLE hSerial_ = INVALID_HANDLE_VALUE;
        std::string port_name_ = "COM5"; // Default COM port
        DWORD baudrate_ = CBR_115200;    // Default baud rate (115200)
        bool initialized_ = false;

        // Background reading thread function
        void read_thread_func()
        {
            uint8_t temp_buffer[256];
            DWORD bytes_read = 0;

            while (!stop_reading_.load())
            {
                // Try to read from serial port
                if (ReadFile(hSerial_, temp_buffer, sizeof(temp_buffer), &bytes_read, NULL))
                {
                    if (bytes_read > 0)
                    {
                        // Add received bytes to buffer
                        std::lock_guard<std::mutex> lock(rx_mutex_);
                        rx_buffer_.insert(rx_buffer_.end(), temp_buffer, temp_buffer + bytes_read);
                    }
                }
                else
                {
                    // Read error - wait a bit before retrying
                    Sleep(10);
                }

                // Small delay to avoid busy-waiting
                Sleep(1);
            }
        }
        /* USER CODE END */

    public:
        LoggerDemo() = default;

        ~LoggerDemo() override
        {
            deinit();
        }

        /* USER CODE BEGIN */
        // Set COM port (e.g., "COM5", "COM3")
        void set_port(const std::string &port)
        {
            port_name_ = port;
        }

        // Set baud rate (e.g., 9600, 115200, 921600)
        void set_baudrate(DWORD baudrate)
        {
            baudrate_ = baudrate;
        }
        /* USER CODE END */

        bool init() override
        {
            if (initialized_)
            {
                return true; // Already initialized
            }

            // Open serial port
            std::string port_path = "\\\\.\\" + port_name_;
            hSerial_ = CreateFileA(
                port_path.c_str(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);

            if (hSerial_ == INVALID_HANDLE_VALUE)
            {
                DWORD error = GetLastError();
                // Error opening port (may not exist or already in use)
                return false;
            }

            // Configure serial port parameters
            DCB dcbSerialParams = {0};
            dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

            if (!GetCommState(hSerial_, &dcbSerialParams))
            {
                CloseHandle(hSerial_);
                hSerial_ = INVALID_HANDLE_VALUE;
                return false;
            }

            // Set baud rate and other parameters
            dcbSerialParams.BaudRate = baudrate_;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(hSerial_, &dcbSerialParams))
            {
                CloseHandle(hSerial_);
                hSerial_ = INVALID_HANDLE_VALUE;
                return false;
            }

            // Set timeouts
            COMMTIMEOUTS timeouts = {0};
            timeouts.ReadIntervalTimeout = 50;
            timeouts.ReadTotalTimeoutConstant = 50;
            timeouts.ReadTotalTimeoutMultiplier = 10;
            timeouts.WriteTotalTimeoutConstant = 50;
            timeouts.WriteTotalTimeoutMultiplier = 10;

            if (!SetCommTimeouts(hSerial_, &timeouts))
            {
                CloseHandle(hSerial_);
                hSerial_ = INVALID_HANDLE_VALUE;
                return false;
            }

            // Purge any existing data
            PurgeComm(hSerial_, PURGE_RXCLEAR | PURGE_TXCLEAR);

            // Start background reading thread
            stop_reading_.store(false);
            read_thread_ = std::thread(&LoggerDemo::read_thread_func, this);

            initialized_ = true;
            return true;
        }

        void deinit() override
        {
            if (!initialized_)
            {
                return;
            }

            // Stop background thread
            stop_reading_.store(true);
            if (read_thread_.joinable())
            {
                read_thread_.join();
            }

            // Close serial port
            if (hSerial_ != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hSerial_);
                hSerial_ = INVALID_HANDLE_VALUE;
            }

            initialized_ = false;
        }

        // Send data over serial port (for two-way communication)
        bool send_data(const uint8_t *data, size_t size) override
        {
            if (!initialized_ || hSerial_ == INVALID_HANDLE_VALUE)
            {
                return false;
            }

            DWORD bytes_written = 0;
            if (!WriteFile(hSerial_, data, static_cast<DWORD>(size), &bytes_written, NULL))
            {
                return false;
            }

            return bytes_written == size;
        }

        // Receive data from serial port
        size_t receive_data(uint8_t *out_buffer, size_t max_bytes) override
        {
            if (!initialized_)
            {
                return 0;
            }

            std::lock_guard<std::mutex> lock(rx_mutex_);

            size_t bytes_to_copy = (rx_buffer_.size() < max_bytes) ? rx_buffer_.size() : max_bytes;

            if (bytes_to_copy > 0)
            {
                std::copy(rx_buffer_.begin(), rx_buffer_.begin() + bytes_to_copy, out_buffer);
                rx_buffer_.erase(rx_buffer_.begin(), rx_buffer_.begin() + bytes_to_copy);
            }

            return bytes_to_copy;
        }

        /* USER CODE BEGIN */
        // Get current buffer size (useful for monitoring)
        size_t get_buffer_size() const
        {
            std::lock_guard<std::mutex> lock(const_cast<std::mutex &>(rx_mutex_));
            return rx_buffer_.size();
        }

        // Check if port is open and initialized
        bool is_open() const
        {
            return initialized_ && hSerial_ != INVALID_HANDLE_VALUE;
        }
        /* USER CODE END */
    };

} // namespace uart_protocol
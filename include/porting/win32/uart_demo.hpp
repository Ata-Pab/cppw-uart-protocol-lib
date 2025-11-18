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
        
    };

} // namespace uart_protocol
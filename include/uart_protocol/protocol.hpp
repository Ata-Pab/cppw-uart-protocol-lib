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

}
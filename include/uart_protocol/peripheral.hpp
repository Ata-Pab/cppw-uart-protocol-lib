#pragma once
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
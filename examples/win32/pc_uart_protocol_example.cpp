#include "uart_protocol/peripheral.hpp"
#include "uart_protocol/protocol.hpp"
#include "porting/win32/uart_demo.hpp"
#include <iostream>
#include <thread>

/*
 * Example implementation of Uart interface for PC (Windows-Port) using placeholder functions.
 * This is a mock implementation for demonstration purposes only.
 */

int main()
{
    // Create two UartDemo instances to simulate producer and receiver, a <-> b cross-connection by moving data between a.tx and b.rx, etc.
    uart_protocol::UartDemo producer_uart;
    uart_protocol::UartDemo receiver_uart;

    producer_uart.init();
    receiver_uart.init();

    uart_protocol::Protocol producer_protocol(producer_uart);
    uart_protocol::Protocol receiver_protocol(receiver_uart);

    producer_protocol.init();
    receiver_protocol.init();

    // SIMULATE Connection: In a real scenario, data sent from producer_uart would be received by receiver_uart.
    // Here we simulate this by transferring data from producer's tx_buffer_ to receiver's rx_buffer_ and reading it by pushing to receiver_uart

    producer_protocol.send_frame(uart_protocol::config::DATA_TYPE, {0xDE, 0xAD, 0xBE, 0xEF});
    // Simulate data transfer from producer to receiver
    auto sent_data = producer_uart.simulate_clear_tx_buffer();
    for (auto byte : sent_data)
    {
        receiver_uart.simulate_incoming_data({byte});
    }

    // Send frame and wait for ACK
    bool ack_received = receiver_protocol.send_frame_wait_ack(uart_protocol::config::DATA_TYPE, {0xDE, 0xAD, 0xBE, 0xEF}, 500);
    if (ack_received)
    {
        std::cout << "ACK received by receiver_protocol." << std::endl;
    }
    else
    {
        std::cout << "ACK NOT received by receiver_protocol." << std::endl;
    }

    producer_protocol.deinit();
    receiver_protocol.deinit();

    std::cout << "UART Protocol Windows Demo is finished." << std::endl;

    std::cin.get();
    return 0;
}
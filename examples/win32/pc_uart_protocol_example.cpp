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
    // Here we simulate this by transferring data from producer's tx_buffer_ to receiver's rx_buffer_

    std::cout << "\n=== Test 1: Simple frame send from producer to receiver ===" << std::endl;
    producer_protocol.send_frame(uart_protocol::config::DATA_TYPE, {0xDE, 0xAD, 0xBE, 0xEF});

    // Simulate data transfer from producer to receiver
    auto sent_data = producer_uart.simulate_clear_tx_buffer();
    for (auto byte : sent_data)
    {
        receiver_uart.simulate_incoming_data({byte});
        std::cout << "Simulated byte 0x" << std::hex << static_cast<int>(byte) << " sent from producer to receiver." << std::endl;
    }
    std::cout << "Frame sent successfully!\n"
              << std::endl;

    // Test 2: Send frame and wait for ACK (with proper ACK simulation)
    // The Test 2 is in the development phase, do not trust its correctness yet
    std::cout << "=== Test 2: Send frame from receiver and wait for ACK ===" << std::endl;

    // Start a thread to simulate the producer responding with ACK
    std::thread ack_responder([&]()
                              {
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate processing delay
        
        // Transfer the frame from receiver to producer
        auto frame_data = receiver_uart.simulate_clear_tx_buffer();
        for (auto byte : frame_data)
        {
            producer_uart.simulate_incoming_data({byte});
            std::cout << "Simulated byte 0x" << std::hex << static_cast<int>(byte) << " sent from receiver to producer." << std::endl;
        }
        
        // Producer sends ACK back
        producer_protocol.send_ack();
        std::cout << "Producer sent ACK." << std::endl;
        
        // Transfer ACK from producer to receiver
        auto ack_data = producer_uart.simulate_clear_tx_buffer();
        for (auto byte : ack_data)
        {
            receiver_uart.simulate_incoming_data({byte});
            std::cout << "Simulated byte 0x" << std::hex << static_cast<int>(byte) << " sent from producer to receiver." << std::endl;
        } } // End of lambda
    );

    bool ack_received = receiver_protocol.send_frame_wait_ack(uart_protocol::config::DATA_TYPE, {0xCA, 0xFE, 0xBA, 0xBE}, 500);

    ack_responder.join(); // Wait for the responder thread to finish
    std::cout<< "Finished waiting task for ACK." << std::endl;    

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
#include "uart_protocol/peripheral.hpp"
#include "uart_protocol/protocol.hpp"
#include "porting/win32/uart_demo.hpp"
#include <iostream>
#include <thread>
#include <atomic>

/*
 * Example implementation of Uart interface for PC (Windows-Port) using placeholder functions.
 * This is a mock implementation for demonstration purposes only.
 * 
 * Producer Uart: Sends frames and waits for ACKs.
 * Receiver Uart: Receives frames and sends ACKs back.
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

    // Test 2: Producer sends frame and waits for ACK, receiver receives and sends ACK back
    std::cout << "=== Test 2: Producer sends frame with ACK (bidirectional communication) ===" << std::endl;

    std::atomic<bool> test_complete{false};

    // Receiver thread: Monitors for incoming frames and sends ACK
    std::thread receiver_thread([&]()
                                {
        std::cout << "Receiver Thread started, waiting for frames..." << std::endl;
        
        std::vector<uint8_t> recv_buffer;
        recv_buffer.reserve(uart_protocol::config::MAX_PAYLOAD_SIZE);
        uint8_t temp_buffer[64];
        
        while (!test_complete.load()) {
            // Simulate data transfer from producer to receiver
            auto data_from_producer = producer_uart.simulate_clear_tx_buffer();
            if (!data_from_producer.empty()) {
                for (auto byte : data_from_producer) {
                    receiver_uart.simulate_incoming_data({byte});
                    std::cout << "Producer transmits 0x" << std::hex << static_cast<int>(byte) << " -> receiver" << std::endl;
                }
            }
            
            // Try to receive data
            size_t n = receiver_uart.receive_data(temp_buffer, sizeof(temp_buffer));
            if (n > 0) {
                recv_buffer.insert(recv_buffer.end(), temp_buffer, temp_buffer + n);
                
                // Try to parse frame
                uart_protocol::Frame received_frame;
                if (uart_protocol::parse_frame(recv_buffer, received_frame)) {
                    std::cout << "Receiver Frame received! Type: 0x" << std::hex << static_cast<int>(received_frame.type) 
                              << ", Payload size: " << std::dec << received_frame.payload.size() << std::endl;
                    
                    // Clear the buffer after successful parse
                    recv_buffer.clear();
                    
                    // Send ACK back
                    std::cout << "Receiver Sending ACK..." << std::endl;
                    receiver_protocol.send_ack();
                }
            }
            
            // Simulate data transfer from receiver to producer (ACK)
            auto data_from_receiver = receiver_uart.simulate_clear_tx_buffer();
            if (!data_from_receiver.empty()) {
                for (auto byte : data_from_receiver) {
                    producer_uart.simulate_incoming_data({byte});
                    std::cout << "Receiver transmits 0x" << std::hex << static_cast<int>(byte) << " -> producer" << std::endl;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        std::cout << "Receiver Thread finished." << std::endl; });

    // Producer thread: sends frame and waits for ACK
    std::thread producer_thread([&]()
                                {
        std::cout << "Producer Thread started, sending frame..." << std::endl;
        
        // Send frame and wait for ACK
        bool ack_received = producer_protocol.send_frame_wait_ack(
            uart_protocol::config::DATA_TYPE, 
            {0xCA, 0xFE, 0xBA, 0xBE}, 
            2000);
        
        if (ack_received) {
            std::cout << "Producer ACK received successfully!" << std::endl;
        } else {
            std::cout << "Producer ACK NOT received (timeout)" << std::endl;
        }
        
        test_complete.store(true);
        std::cout << "Producer Thread finished." << std::endl; });

    // Wait for both threads to complete
    producer_thread.join();
    receiver_thread.join();

    std::cout << "\n=== Test 2 Complete ===" << std::endl;

    producer_protocol.deinit();
    receiver_protocol.deinit();

    std::cout << "UART Protocol Windows Demo is finished." << std::endl;

    std::cin.get();
    return 0;
}
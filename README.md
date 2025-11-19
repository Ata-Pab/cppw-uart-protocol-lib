# Modern, Performant, Global & Efficient C++ UART Protocol Library

This repo contains a C++ library for implementing a modern UART communication protocol with a focus on performance, modularity, and ease of use across various platforms including embedded systems and PC applications.

## Structure

    uart-protocol-lib/
    ├─ CMakeLists.txt
    ├─ include/
    │  ├─ uart_protocol/
    │  │  ├─ peripheral.hpp
    │  │  ├─ protocol.hpp
    │  │  ├─ protocol_config.hpp        
    │  │  └─ frame_utility.hpp
    │  ├─ porting/
    │  │  ├─ win32/
    │  │  │  └─ uart_demo.hpp
    ├─ src/
    │  └─ (empty for now; platform-specific implementations live in platform)
    ├─ tests/
    │  ├─ CMakeLists.txt
    │  └─ test_protocol.cpp
    ├─ examples/
    |  ├─ win32/
    │  │  └─ pc_uart_protocol_example.cpp
    │  └─ embedded_adapter_stub.cpp
    └─ README.md


## Design Principles

- **Modularity**: Clear separation between protocol logic and UART interface.
- **Performance**: Optimized for low latency and high throughput.
- **Global Compatibility**: Designed to work across different platforms and architectures.
- **Ease of Use**: Simple API for quick integration into existing projects.

## Building

This project uses CMake and FetchContent to pull [GoogleTest](https://github.com/google/googletest) and Google Benchmark for tests and benchmarks.

### Basic build (out-of-source):

```powershell
mkdir build; cd build
cmake ..
cmake --build . --config Release
```

### Run the main.cpp (Locale Test Example)
```powershell
.\source\Release\main.exe
```

Or if using Debug configuration:

```powershell
cmake --build . --config Debug
.\source\Debug\main.exe
```

### Run the Google Tests
```powershell
cmake -B build -S .
cmake --build build
ctest --test-dir build
ctest -C Release --output-on-failure
```

### Run benchmarks (after building):

```powershell
cmake -B build -S .
cmake --build build
./build/uart_protocol_benchmarks
```

## Concept/Overview

### Protocol Frame Structure

> [ START_WORD (2 bytes) ]  
> [ TYPE (1 byte) ]  
> [ LEN  (1 byte) ]  
> [ PAYLOAD (0–255 bytes) ]  
> [ CRC (1 byte) ]

### Protocol Flow Sequence

Typical flow example **between a PC and an Embedded Device**.

> PC is the master.


#### 1. Master sends START frame

```cpp
protocol.send_start_word();
```

This builds a frame:

```
AA 55      ← start word (delimiter)
01         ← TYPE = START_WORD_TYPE
00         ← LEN = 0
XX         ← CRC
```

---

#### 2. Device receives the frame and checks TYPE

Firmware on the embedded device parses the frame:

```
TYPE == START_WORD_TYPE?
```

If true → device replies with an ACK.

#### 3. Device responds with ACK

```cpp
protocol.send_ack();
```

Which builds and sends:

```
AA 55      ← frame start delimiter  
02         ← TYPE = ACK_TYPE  
00         ← LEN = 0  
YY         ← CRC  
```

---

#### 4. Master waits for ACK

When the master uses:

```cpp
protocol.send_frame_wait_ack(...)
```

This sequence happens internally:

### MASTER:

1. Send frame
2. Start timeout timer
3. Read UART in small chunks
4. Append bytes to buffer
5. Call `parse_frame()`
6. If parsed frame has TYPE==ACK_TYPE → success
7. If timeout → failure

---

## Example Usage

```cpp
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
```

## Output

```
=== Test 1: Simple frame send from producer to receiver ===
Simulated byte 0x55 sent from producer to receiver.
Simulated byte 0xaa sent from producer to receiver.
Simulated byte 0x4 sent from producer to receiver.
Simulated byte 0x4 sent from producer to receiver.
Simulated byte 0xde sent from producer to receiver.
Simulated byte 0xad sent from producer to receiver.
Simulated byte 0xbe sent from producer to receiver.
Simulated byte 0xef sent from producer to receiver.
Simulated byte 0x55 sent from producer to receiver.
Simulated byte 0xcf sent from producer to receiver.
Frame sent successfully!

=== Test 2: Send frame from receiver and wait for ACK ===
Simulated byte 0x55 sent from receiver to producer.
Simulated byte 0xaa sent from receiver to producer.
Simulated byte 0x4 sent from receiver to producer.
Simulated byte 0x4 sent from receiver to producer.
Simulated byte 0xca sent from receiver to producer.
Simulated byte 0xfe sent from receiver to producer.
Simulated byte 0xba sent from receiver to producer.
Simulated byte 0xbe sent from receiver to producer.
Simulated byte 0x8d sent from receiver to producer.
Simulated byte 0x9f sent from receiver to producer.
Producer sent ACK.
Simulated byte 0x55 sent from producer to receiver.
Simulated byte 0xaa sent from producer to receiver.
Simulated byte 0x0 sent from producer to receiver.
Simulated byte 0x2 sent from producer to receiver.
Simulated byte 0xa1 sent from producer to receiver.
Simulated byte 0x17 sent from producer to receiver.
Finished waiting task for ACK.
ACK received by receiver_protocol.
UART Protocol Windows Demo is finished.
```


## Troubleshooting




## Reference

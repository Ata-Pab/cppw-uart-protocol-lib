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
#include "peripheral.hpp"
#include "protocol.hpp"
#include "uart_demo.hpp"
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
}
```

## Troubleshooting




## Reference

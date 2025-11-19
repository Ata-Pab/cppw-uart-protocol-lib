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

## Example Usage

```cpp

```

## Troubleshooting




## Reference

cmake_minimum_required(VERSION 3.14)
project(cppw-uart-protocol-lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(uart_protocol_lib INTERFACE)
target_include_directories(uart_protocol_lib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Build example executable
add_executable(pc_uart_protocol_example 
    examples/win32/pc_uart_protocol_example.cpp
)
target_link_libraries(pc_uart_protocol_example PRIVATE uart_protocol_lib)

# Build logger example executable
add_executable(pc_logger_example 
    examples/win32/pc_logger_example.cpp
)
target_link_libraries(pc_logger_example PRIVATE uart_protocol_lib)

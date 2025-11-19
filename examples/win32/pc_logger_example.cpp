#include "porting/win32/logger_demo.hpp"
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

/*
 * Serial Logger Application
 *
 * This application receives and displays log data from a transmitter device
 * connected via USB (COM port). It continuously reads from the serial port
 * and displays the received data in real-time.
 *
 * Configuration:
 * - Default Port: COM5 (change if needed)
 * - Default Baud: 115200 (change if needed)
 */

// Helper function to get current timestamp
std::string get_timestamp()
{
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "Serial Logger Application" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Create and configure serial port logger
    uart_protocol::LoggerDemo logger;

    // Configure COM port
    std::string com_port = "COM5";
    std::cout << "Enter COM port [default: COM5]: ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty())
    {
        com_port = input;
    }
    logger.set_port(com_port);

    // Configure baud rate
    DWORD baudrate = 115200;
    std::cout << "Enter baud rate [default: 115200]: ";
    std::getline(std::cin, input);
    if (!input.empty())
    {
        try
        {
            baudrate = std::stoul(input);
        }
        catch (...)
        {
            std::cout << "Invalid baud rate, using default 115200" << std::endl;
            baudrate = 115200;
        }
    }
    logger.set_baudrate(baudrate);

    // Initialize serial port
    std::cout << std::endl;
    std::cout << "Opening " << com_port << " at " << baudrate << " baud..." << std::endl;

    if (!logger.init())
    {
        std::cerr << "ERROR: Failed to open " << com_port << std::endl;
        std::cerr << "Please check:" << std::endl;
        std::cerr << "  1. Transmitter device is connected via USB" << std::endl;
        std::cerr << "  2. Correct COM port (check Device Manager)" << std::endl;
        std::cerr << "  3. No other program is using the port" << std::endl;
        std::cout << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    std::cout << "SUCCESS: Port opened successfully!" << std::endl;
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Receiving data from device..." << std::endl;
    std::cout << "  Press Ctrl+C to stop" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Buffer for received data
    uint8_t buffer[512];
    std::string line_buffer;

    // Main receive loop
    while (true)
    {
        // Read available data
        size_t bytes_read = logger.receive_data(buffer, sizeof(buffer));

        if (bytes_read > 0)
        {

            // Process received bytes
            for (size_t i = 0; i < bytes_read; i++)
            {
                char c = static_cast<char>(buffer[i]);

                // Check for newline
                if (c == '\n' || c == '\r')
                {
                    if (!line_buffer.empty())
                    {
                        // Display complete line with timestamp
                        std::cout << "[" << get_timestamp() << "] " << line_buffer << std::endl;
                        line_buffer.clear();
                    }
                }
                else if (c >= 32 && c <= 126) // Printable ASCII characters
                {
                    line_buffer += c;
                }
            }
        }

        // Small delay to avoid busy-waiting
        Sleep(10);
    }

    logger.deinit();
    return 0;
}

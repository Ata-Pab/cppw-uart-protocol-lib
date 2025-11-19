#pragma once
#include <cstdint>

// Include platform-specific headers
#if defined(USE_FREERTOS) /* FreeRTOS Implementation */
#include "FreeRTOS.h"
#include "task.h"
#elif defined(USE_STD_CHRONO) /* Standard C++ Implementation (PC) */
#include <chrono>
#include <thread>
#elif defined(BARE_METAL) /* Custom: Platform-specific includes & definitions */
/* USER CODE BEGIN */

/* USER CODE END */
#else
#error "No timing platform defined. Define USE_FREERTOS, USE_STD_CHRONO, or BARE_METAL"
#endif

/*
 * Timing Utility - Platform abstraction for timing and delay functions.
 *
 * This header provides a unified interface for timing operations across different platforms.
 * The implementation is selected at compile-time using preprocessor directives.
 *
 * Supported Platforms:
 * - USE_FREERTOS: FreeRTOS-based systems (define this before including)
 * - USE_STD_CHRONO: Standard C++ with <chrono> (default for PC/desktop)
 * - BARE_METAL: Custom implementations (user must provide get_tick_ms and delay_ms)
 *
 * Usage:
 * - timing::get_tick_ms() -> Returns current time in milliseconds
 * - timing::delay_ms(ms) -> Delays for specified milliseconds
 * - timing::has_elapsed(start, duration) -> Checks if duration has passed since start
 */

namespace uart_protocol::timing
{
    // ============================================================================
    // Platform-Specific Implementations
    // ============================================================================

#if defined(USE_FREERTOS)
    /* FreeRTOS Implementation */
    inline uint32_t get_tick_ms()
    {
        return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
    }

    inline void delay_ms(uint32_t ms)
    {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }

#elif defined(USE_STD_CHRONO) || !defined(BARE_METAL)
    /* Standard C++ Implementation (PC) */
    inline uint32_t get_tick_ms()
    {
        using namespace std::chrono;
        return static_cast<uint32_t>(
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
    }

    inline void delay_ms(uint32_t ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

#elif defined(BARE_METAL)
    /* Bare-Metal Implementation (Custom) */
    /*
     * For bare-metal systems, you must provide these functions in your project:
     *
     * uint32_t uart_protocol::timing::get_tick_ms() {
     *     // Return current tick count in milliseconds
     *     // Example: return HAL_GetTick(); for STM32
     * }
     *
     * void uart_protocol::timing::delay_ms(uint32_t ms) {
     *     // Implement delay in milliseconds
     *     // Example: HAL_Delay(ms); for STM32
     * }
     */

    // Forward declarations - must be implemented by user
    uint32_t get_tick_ms();
    void delay_ms(uint32_t ms);

#else
#error "No timing platform defined. Define USE_FREERTOS, USE_STD_CHRONO, or BARE_METAL"
#endif
    // Common Utility Functions (Platform-Independent)

    /*
     * Check if a specified duration has elapsed since a start time.
     * Handles uint32_t overflow correctly.
     *
     * @param start_ms Start time in milliseconds (from get_tick_ms)
     * @param duration_ms Duration to check in milliseconds
     * @return true if duration has elapsed, false otherwise
     */
    inline bool has_elapsed(uint32_t start_ms, uint32_t duration_ms)
    {
        uint32_t current_ms = get_tick_ms();
        uint32_t elapsed = current_ms - start_ms; // Handles overflow automatically
        return elapsed >= duration_ms;
    }

    /*
     * Get elapsed time since a start time.
     * Handles uint32_t overflow correctly.
     *
     * @param start_ms Start time in milliseconds (from get_tick_ms)
     * @return Elapsed time in milliseconds
     */
    inline uint32_t get_elapsed(uint32_t start_ms)
    {
        return get_tick_ms() - start_ms; // Handles overflow automatically
    }

} // namespace uart_protocol::timing

#include <cstdio>
#include <cstdint>

#include "systick_regs.hpp"

extern "C" void initialise_monitor_handles();

namespace {
volatile std::uint32_t g_systick_ticks = 0;
}

extern "C" void SysTick_Handler() {
    g_systick_ticks = g_systick_ticks + 1u;
}

extern "C" int main() {
    initialise_monitor_handles();
    std::puts("Hello, Cortex-M from C++20 via semihosting!");

    mcu::systick::SysTick systick;
    constexpr std::uint32_t reload_value = 800000u - 1u;

    systick.configure(reload_value, true, true);

    __asm volatile("cpsie i" ::: "memory");
    systick.start();

    std::uint32_t last_reported = 0;
    bool first_tick_reported = false;
    std::uint32_t last_current = systick.current();
    bool fallback_reported = false;
    std::uint32_t wraps = 0;

    while (true) {
        if (!first_tick_reported && g_systick_ticks > 0u) {
            first_tick_reported = true;
            std::puts("SysTick first tick");
        }

        if (!fallback_reported) {
            const std::uint32_t now_current = systick.current();
            if (now_current != last_current) {
                fallback_reported = true;
                std::puts("SysTick counter is running");
            }
            last_current = now_current;
        }

        if (systick.wrapped()) {
            ++wraps;
        }

        if ((g_systick_ticks - last_reported) >= 10u || wraps >= 10u) {
            last_reported = g_systick_ticks;
            wraps = 0;
            std::puts("SysTick heartbeat");
        }
    }
}

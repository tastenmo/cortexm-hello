#include <cstdio>

extern "C" void initialise_monitor_handles();

extern "C" int main() {
    initialise_monitor_handles();
    std::puts("Hello, Cortex-M from C++20 via semihosting!");

    while (true) {
    }
}

#include <cstdint>

extern "C" {
extern std::uint32_t _estack;
extern std::uint32_t _sidata;
extern std::uint32_t _sdata;
extern std::uint32_t _edata;
extern std::uint32_t _sbss;
extern std::uint32_t _ebss;

int main();
void __libc_init_array();
void _init();

void Reset_Handler();
void Default_Handler();
void NMI_Handler() __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler() __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler() __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler() __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler() __attribute__((weak, alias("Default_Handler")));
void SVC_Handler() __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler() __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler() __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler() __attribute__((weak, alias("Default_Handler")));

using isr_t = void (*)();

__attribute__((section(".isr_vector"), used))
const isr_t vector_table[] = {
    reinterpret_cast<isr_t>(&_estack),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    SVC_Handler,
    DebugMon_Handler,
    nullptr,
    PendSV_Handler,
    SysTick_Handler,
};

void Reset_Handler() {
    auto* src = &_sidata;
    for (auto* dst = &_sdata; dst < &_edata;) {
        *dst++ = *src++;
    }

    for (auto* dst = &_sbss; dst < &_ebss;) {
        *dst++ = 0;
    }

    __libc_init_array();
    (void)main();

    while (true) {
    }
}

void Default_Handler() {
    while (true) {
    }
}

void _init() {
}
}  // extern "C"

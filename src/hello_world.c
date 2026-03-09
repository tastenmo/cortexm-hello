#include <stdio.h>
#include "pico/stdlib.h"

int main() {
stdio_init_all(); // Works for both USB and UART
while (true) {
printf("Hello, world!\n");
sleep_ms(1000);
}
}
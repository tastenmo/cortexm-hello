# Cortex-M C++20 Hello World (QEMU)

Minimal bare-metal C++20 project for toolchain + QEMU validation on a Cortex-M3 target using semihosting output.

## Requirements

- `arm-none-eabi-g++` toolchain
- `qemu-system-arm`
- `make`

## Build

```bash
make build
make size
```

## Run in QEMU

```bash
make run
```

Expected terminal output includes:

```text
Hello, Cortex-M from C++20 via semihosting!
```

## Notes

- Machine target is `lm3s6965evb` (Cortex-M3).
- Entry/reset comes from `src/startup.cpp` vector table.
- Linker script is `linker.ld`.

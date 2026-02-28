CROSS      ?= arm-none-eabi
CXX        := $(CROSS)-g++
OBJCOPY    := $(CROSS)-objcopy
SIZE       := $(CROSS)-size

BUILD_DIR  := build
TARGET     := $(BUILD_DIR)/hello
BUILD_STAMP := $(BUILD_DIR)/.dir

SRC        := src/startup.cpp src/main.cpp
OBJ        := $(SRC:src/%.cpp=$(BUILD_DIR)/%.o)

CPU_FLAGS  := -mcpu=cortex-m3 -mthumb
CXXFLAGS   := $(CPU_FLAGS) -std=c++20 -O0 -g3 -ffreestanding -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti -Wall -Wextra
LDFLAGS    := $(CPU_FLAGS) -T linker.ld -specs=rdimon.specs -Wl,--gc-sections -Wl,-Map=$(TARGET).map -Wl,--entry=Reset_Handler -nostartfiles

.PHONY: all build run clean size

all: build

build: $(TARGET).elf $(TARGET).bin


$(BUILD_STAMP):
	mkdir -p $(BUILD_DIR)
	touch $(BUILD_STAMP)

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_STAMP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

size: $(TARGET).elf
	$(SIZE) $<

run: $(TARGET).elf
	qemu-system-arm -machine lm3s6965evb -cpu cortex-m3 -nographic -semihosting-config enable=on,target=native -kernel $<

clean:
	rm -rf $(BUILD_DIR)

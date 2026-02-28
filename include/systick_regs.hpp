#pragma once

#include <bit>
#include <cstdint>
#include <span>

#include <RegisterPack.h>

namespace mcu::systick {

inline constexpr std::uintptr_t base_address = 0xE000E010UL;

struct Registers : regs::RegisterPack<16> {
  using RegisterPack::RegisterPack;
};

struct Ctrl;
struct Ctrl : regs::PackedRegister<Registers, Ctrl, 0, std::uint32_t> {
  using PackedRegister::PackedRegister;

  using ENABLE = regs::Field<Ctrl, 0, 1>;
  using TICKINT = regs::Field<Ctrl, 1, 1>;
  using CLKSOURCE = regs::Field<Ctrl, 2, 1>;
  using COUNTFLAG = regs::Field<Ctrl, 16, 1, 0, regs::read_only>;
};

struct Load;
struct Load : regs::PackedRegister<Registers, Load, 4, std::uint32_t> {
  using PackedRegister::PackedRegister;

  using RELOAD = regs::Field<Load, 0, 32>;
};

struct Value;
struct Value : regs::PackedRegister<Registers, Value, 8, std::uint32_t> {
  using PackedRegister::PackedRegister;

  using CURRENT = regs::Field<Value, 0, 32>;
};

struct Calib;
struct Calib : regs::PackedRegister<Registers, Calib, 12, std::uint32_t> {
  using PackedRegister::PackedRegister;

  using TENMS = regs::Field<Calib, 0, 32, 0, regs::read_only>;
  using SKEW = regs::Field<Calib, 30, 1, 0, regs::read_only>;
  using NOREF = regs::Field<Calib, 31, 1, 0, regs::read_only>;
};

class SysTick {
 public:
  explicit SysTick(std::uintptr_t base = base_address)
      : base_(base) {}

  void set_reload(std::uint32_t reload_value) {
    auto raw = read_reg(load_offset);
    raw = write_field<Load::RELOAD>(raw, reload_value & 0x00FFFFFFu);
    write_reg(load_offset, raw);
  }

  void clear_current() {
    auto raw = read_reg(value_offset);
    raw = write_field<Value::CURRENT>(raw, 0u);
    write_reg(value_offset, raw);
  }

  void set_interrupt_enabled(bool enabled) {
    auto raw = read_reg(ctrl_offset);
    raw = write_field<Ctrl::TICKINT>(raw, enabled ? 1u : 0u);
    write_reg(ctrl_offset, raw);
  }

  void set_core_clock_source(bool use_core_clock) {
    auto raw = read_reg(ctrl_offset);
    raw = write_field<Ctrl::CLKSOURCE>(raw, use_core_clock ? 1u : 0u);
    write_reg(ctrl_offset, raw);
  }

  void start() {
    auto raw = read_reg(ctrl_offset);
    raw = write_field<Ctrl::ENABLE>(raw, 1u);
    write_reg(ctrl_offset, raw);
  }

  void stop() {
    auto raw = read_reg(ctrl_offset);
    raw = write_field<Ctrl::ENABLE>(raw, 0u);
    write_reg(ctrl_offset, raw);
  }

  void configure(std::uint32_t reload_value, bool enable_interrupt = false,
                 bool use_core_clock = true) {
    stop();
    set_reload(reload_value);
    clear_current();
    set_interrupt_enabled(enable_interrupt);
    set_core_clock_source(use_core_clock);
  }

  std::uint32_t current() {
    return read_field<Value::CURRENT>(read_reg(value_offset)) & 0x00FFFFFFu;
  }

  bool wrapped() {
    return read_field<Ctrl::COUNTFLAG>(read_reg(ctrl_offset)) != 0u;
  }

  std::uint32_t calib_10ms() {
    return read_field<Calib::TENMS>(read_reg(calib_offset)) & 0x00FFFFFFu;
  }

 private:
  static constexpr std::uintptr_t ctrl_offset = 0x00u;
  static constexpr std::uintptr_t load_offset = 0x04u;
  static constexpr std::uintptr_t value_offset = 0x08u;
  static constexpr std::uintptr_t calib_offset = 0x0Cu;

  template <typename FieldType>
  static typename FieldType::value_type read_field(std::uint32_t raw) {
    auto bytes = std::bit_cast<regs::byte_array<sizeof(std::uint32_t)>>(raw);
    return FieldType::read(std::span<const std::byte>(bytes.begin(), bytes.size()));
  }

  template <typename FieldType>
  static std::uint32_t write_field(std::uint32_t raw,
                                   typename FieldType::value_type value) {
    auto bytes = std::bit_cast<regs::byte_array<sizeof(std::uint32_t)>>(raw);
    FieldType::write(std::span<std::byte>(bytes.begin(), bytes.size()), value);
    return std::bit_cast<std::uint32_t>(bytes);
  }

  std::uint32_t read_reg(std::uintptr_t offset) const {
    auto* const reg = reinterpret_cast<volatile std::uint32_t*>(base_ + offset);
    return *reg;
  }

  void write_reg(std::uintptr_t offset, std::uint32_t value) const {
    auto* const reg = reinterpret_cast<volatile std::uint32_t*>(base_ + offset);
    *reg = value;
  }

  std::uintptr_t base_;
};

} // namespace mcu::systick

#include "memory.hpp"

#include <cstdint>

uint16_t Memory::get_word(uint16_t addr) const {
  const uint8_t lo = get_byte(addr);
  const uint8_t hi = get_byte(addr + 1);
  return (static_cast<uint16_t>(hi) << 8) | lo;
}

void Memory::set_word(uint16_t addr, uint16_t value) {
  const uint8_t lo = value & 0xFF;
  const uint8_t hi = (value >> 8) & 0xFF;
  set_byte(addr, lo);
  set_byte(addr + 1, hi);
}

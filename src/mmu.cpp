#include "mmu.hpp"

#include "cartridge.hpp"
#include "memory.hpp"

uint8_t MMU::get_byte(uint16_t addr) const {
  if (0x0000 <= addr && addr <= 0x7fff) {
    // ROM
    return cartridge->get_byte(addr);
  } else if (0x8000 <= addr && addr <= 0x9fff) {
    // VRAM (not implemented)
    return 0;
  } else if (0xa000 <= addr && addr <= 0xbfff) {
    // External RAM
    return cartridge->get_byte(addr);
  } else if (0xc000 <= addr && addr <= 0xdfff) {
    // WRAM (not implemented)
    return 0;
  } else if (0xe000 <= addr && addr <= 0xfdff) {
    // Echo RAM (not implemented)
    return 0;
  } else if (0xfe00 <= addr && addr <= 0xfe9f) {
    // OAM (not implemented)
    return 0;
  } else if (0xff00 <= addr && addr <= 0xff7f) {
    // I/O Registers (not implemented)
    return 0;
  } else if (0xff80 <= addr && addr <= 0xfffe) {
    // HRAM (not implemented)
    return 0;
  } else if (addr == 0xffff) {
    // Interrupt Enable Register (not implemented)
    return 0;
  }
}

void MMU::set_byte(uint16_t addr, uint8_t value) {
  if (0x0000 <= addr && addr <= 0x7fff) {
    // ROM
    cartridge->set_byte(addr, value);
  } else if (0x8000 <= addr && addr <= 0x9fff) {
    // VRAM (not implemented)
  } else if (0xa000 <= addr && addr <= 0xbfff) {
    cartridge->set_byte(addr, value);
  } else if (0xc000 <= addr && addr <= 0xdfff) {
    // WRAM (not implemented)
  } else if (0xe000 <= addr && addr <= 0xfdff) {
    // Echo RAM (not implemented)
  } else if (0xfe00 <= addr && addr <= 0xfe9f) {
    // OAM (not implemented)
  } else if (0xff00 <= addr && addr <= 0xff7f) {
    // I/O Registers (not implemented)
  } else if (0xff80 <= addr && addr <= 0xfffe) {
    // HRAM (not implemented)
  } else if (addr == 0xffff) {
    // Interrupt Enable Register (not implemented)
  }
}

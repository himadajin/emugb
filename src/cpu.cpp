#include "cpu.hpp"

#include "memory.hpp"
#include "register.hpp"
#include <print>

uint8_t CPU::imm_byte() {
  const uint8_t value = memory.get_byte(regFile.pc);
  regFile.pc += 1;
  return value;
}

uint16_t CPU::imm_word() {
  const uint16_t value = memory.get_word(regFile.pc);
  regFile.pc += 2;
  return value;
}

void CPU::execute() {
  const uint8_t byte0 = imm_byte();
  switch (byte0) {
  // NOP
  case 0x00:
    std::println("NOP");
    break;

  // LD r16, imm16
  case 0x01: {
    const uint16_t imm16 = imm_word();
    regFile.set_bc(imm16);
    std::println("LD BC, 0x{:04X}", imm16);
    break;
  }
  case 0x11: {
    const uint16_t imm16 = imm_word();
    regFile.set_de(imm16);
    std::println("LD DE, 0x{:04X}", imm16);
    break;
  }
  case 0x21: {
    const uint16_t imm16 = imm_word();
    regFile.set_hl(imm16);
    std::println("LD HL, 0x{:04X}", imm16);
    break;
  }
  case 0x31: {
    const uint16_t imm16 = imm_word();
    regFile.sp = imm16;
    std::println("LD SP, 0x{:04X}", imm16);
    break;
  }

  // LD (r16), A
  case 0x02: {
    const uint16_t addr = regFile.get_bc();
    memory.set_byte(addr, regFile.a);
    std::println("LD (BC), A");
    break;
  }
  case 0x12: {
    const uint16_t addr = regFile.get_de();
    memory.set_byte(addr, regFile.a);
    std::println("LD (DE), A");
    break;
  }
  case 0x22: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.a);
    regFile.set_hl(addr + 1);
    std::println("LD (HL+), A");
    break;
  }
  case 0x32: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.a);
    regFile.set_hl(addr - 1);
    std::println("LD (HL-), A");
    break;
  }

  // LD A, (r16)
  case 0x0A: {
    const uint16_t addr = regFile.get_bc();
    regFile.a = memory.get_byte(addr);
    std::println("LD A, (BC)");
    break;
  }
  case 0x1A: {
    const uint16_t addr = regFile.get_de();
    regFile.a = memory.get_byte(addr);
    std::println("LD A, (DE)");
    break;
  }
  case 0x2A: {
    const uint16_t addr = regFile.get_hl();
    regFile.a = memory.get_byte(addr);
    regFile.set_hl(addr + 1);
    std::println("LD A, (HL+)");
    break;
  }
  case 0x3A: {
    const uint16_t addr = regFile.get_hl();
    regFile.a = memory.get_byte(addr);
    regFile.set_hl(addr - 1);
    std::println("LD A, (HL-)");
    break;
  }

  // LD (imm16), SP
  case 0x08: {
    const uint16_t addr = imm_word();
    memory.set_word(addr, regFile.sp);
    std::println("LD (0x{:04X}), SP", addr);
    break;
  }

  // INC r16
  case 0x03: {
    const uint16_t value = regFile.get_bc();
    regFile.set_bc(value + 1);
    std::println("INC BC");
    break;
  }
  case 0x13: {
    const uint16_t value = regFile.get_de();
    regFile.set_de(value + 1);
    std::println("INC DE");
    break;
  }
  case 0x23: {
    const uint16_t value = regFile.get_hl();
    regFile.set_hl(value + 1);
    std::println("INC HL");
    break;
  }
  case 0x33: {
    regFile.sp += 1;
    std::println("INC SP");
    break;
  }

  // DEC r16
  case 0x0B: {
    const uint16_t value = regFile.get_bc();
    regFile.set_bc(value - 1);
    std::println("DEC BC");
    break;
  }
  case 0x1B: {
    const uint16_t value = regFile.get_de();
    regFile.set_de(value - 1);
    std::println("DEC DE");
    break;
  }
  case 0x2B: {
    const uint16_t value = regFile.get_hl();
    regFile.set_hl(value - 1);
    std::println("DEC HL");
    break;
  }
  case 0x3B: {
    regFile.sp -= 1;
    std::println("DEC SP");
    break;
  }
  default:
    std::println(stderr,
                 "Error: Unknown opcode found (PC: 0x{:04X} OPCODE: 0x{:02X})",
                 regFile.pc - 1, byte0);
    break;
  }
}

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
  default:
    std::println(stderr,
                 "Error: Unknown opcode found (PC: 0x{:04X} OPCODE: 0x{:02X})",
                 regFile.pc - 1, byte0);
    break;
  }
}

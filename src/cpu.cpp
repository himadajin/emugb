#include "cpu.hpp"

#include "memory.hpp"
#include "register.hpp"
#include <print>

uint8_t CPU::imm_byte() {
  const uint8_t value = memory.get_byte(regFile.pc);
  regFile.pc += 1;
  return value;
}

void CPU::execute() {
  const uint8_t byte0 = imm_byte();
  switch (byte0) {
  // NOP
  case 0x00:
    std::println("NOP");
    break;
  default:
    std::println(stderr,
                 "Error: Unknown opcode found (PC: 0x{:04X} OPCODE: 0x{:02X})",
                 regFile.pc - 1, byte0);
    break;
  }
}

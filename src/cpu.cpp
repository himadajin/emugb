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

uint8_t CPU::alu_inc(uint8_t imm8) {
  const uint8_t result = imm8 + 1;
  regFile.set_flag(Flag::Z, result == 0);
  regFile.set_flag(Flag::H, (imm8 & 0x0F) + 1 > 0x0F);
  regFile.set_flag(Flag::N, false);
  // Flag::C is not affected.
  return result;
}

uint8_t CPU::alu_dec(uint8_t imm8) {
  const uint8_t result = imm8 - 1;
  regFile.set_flag(Flag::Z, result == 0);
  regFile.set_flag(Flag::H, (imm8 & 0x0F) == 0);
  regFile.set_flag(Flag::N, true);
  // Flag::C is not affected.
  return result;
}

void CPU::alu_add_hl(uint16_t imm16) {
  const uint32_t result =
      static_cast<uint32_t>(regFile.get_hl()) + static_cast<uint32_t>(imm16);
  // Flag::Z is not affected.
  regFile.set_flag(Flag::N, false);
  regFile.set_flag(Flag::H,
                   (regFile.get_hl() & 0x0FFF) + (imm16 & 0x0FFF) > 0x0FFF);
  regFile.set_flag(Flag::C, result > 0xFFFF);
  regFile.set_hl(static_cast<uint16_t>(result & 0xFFFF));
}

// JR: Jump Relative
void CPU::alu_jr(uint8_t imm8) {
  const int32_t offset = static_cast<int8_t>(imm8);
  regFile.pc = static_cast<uint16_t>(static_cast<int32_t>(regFile.pc) + offset);
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

  // ADD HL, r16
  case 0x09: {
    alu_add_hl(regFile.get_bc());
    std::println("ADD HL, BC");
    break;
  }
  case 0x19: {
    alu_add_hl(regFile.get_de());
    std::println("ADD HL, DE");
    break;
  }
  case 0x29: {
    alu_add_hl(regFile.get_hl());
    std::println("ADD HL, HL");
    break;
  }
  case 0x39: {
    alu_add_hl(regFile.sp);
    std::println("ADD HL, SP");
    break;
  }

  // INC r8
  case 0x04: {
    regFile.b = alu_inc(regFile.b);
    std::println("INC B");
    break;
  }
  case 0x14: {
    regFile.d = alu_inc(regFile.d);
    std::println("INC D");
    break;
  }
  case 0x24: {
    regFile.h = alu_inc(regFile.h);
    std::println("INC H");
    break;
  }
  case 0x34: {
    const uint16_t addr = regFile.get_hl();
    const uint8_t value = memory.get_byte(addr);
    const uint8_t result = alu_inc(value);
    memory.set_byte(addr, result);
    std::println("INC (HL)");
    break;
  }
  case 0x0C: {
    regFile.c = alu_inc(regFile.c);
    std::println("INC C");
    break;
  }
  case 0x1C: {
    regFile.e = alu_inc(regFile.e);
    std::println("INC E");
    break;
  }
  case 0x2C: {
    regFile.l = alu_inc(regFile.l);
    std::println("INC L");
    break;
  }
  case 0x3C: {
    regFile.a = alu_inc(regFile.a);
    std::println("INC A");
    break;
  }

  // DEC r8
  case 0x05: {
    regFile.b = alu_dec(regFile.b);
    std::println("DEC B");
    break;
  }
  case 0x15: {
    regFile.d = alu_dec(regFile.d);
    std::println("DEC D");
    break;
  }
  case 0x25: {
    regFile.h = alu_dec(regFile.h);
    std::println("DEC H");
    break;
  }
  case 0x35: {
    const uint16_t addr = regFile.get_hl();
    const uint8_t value = memory.get_byte(addr);
    const uint8_t result = alu_dec(value);
    memory.set_byte(addr, result);
    std::println("DEC (HL)");
    break;
  }
  case 0x0D: {
    regFile.c = alu_dec(regFile.c);
    std::println("DEC C");
    break;
  }
  case 0x1D: {
    regFile.e = alu_dec(regFile.e);
    std::println("DEC E");
    break;
  }
  case 0x2D: {
    regFile.l = alu_dec(regFile.l);
    std::println("DEC L");
    break;
  }
  case 0x3D: {
    regFile.a = alu_dec(regFile.a);
    std::println("DEC A");
    break;
  }

  // LD r8, imm8
  case 0x06: {
    const uint8_t imm8 = imm_byte();
    regFile.b = imm8;
    std::println("LD B, 0x{:02X}", imm8);
    break;
  }
  case 0x16: {
    const uint8_t imm8 = imm_byte();
    regFile.d = imm8;
    std::println("LD D, 0x{:02X}", imm8);
    break;
  }
  case 0x26: {
    const uint8_t imm8 = imm_byte();
    regFile.h = imm8;
    std::println("LD H, 0x{:02X}", imm8);
    break;
  }
  case 0x36: {
    const uint8_t imm8 = imm_byte();
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, imm8);
    std::println("LD (HL), 0x{:02X}", imm8);
    break;
  }
  case 0x0E: {
    const uint8_t imm8 = imm_byte();
    regFile.c = imm8;
    std::println("LD C, 0x{:02X}", imm8);
    break;
  }
  case 0x1E: {
    const uint8_t imm8 = imm_byte();
    regFile.e = imm8;
    std::println("LD E, 0x{:02X}", imm8);
    break;
  }
  case 0x2E: {
    const uint8_t imm8 = imm_byte();
    regFile.l = imm8;
    std::println("LD L, 0x{:02X}", imm8);
    break;
  }
  case 0x3E: {
    const uint8_t imm8 = imm_byte();
    regFile.a = imm8;
    std::println("LD A, 0x{:02X}", imm8);
    break;
  }

  // todo: implement RLCA
  // todo: implement RRCA
  // todo: implement RLA
  // todo: implement RRA
  // todo: implement DAA
  // todo: implement CPL
  // todo: implement SCF
  // todo: implement CCF

  // JR imm8
  case 0x18: {
    const uint8_t imm8 = imm_byte();
    alu_jr(imm8);
    std::println("JR 0x{:02X}", imm8);
    break;
  }

  // JR cond, imm8
  case 0x20: {
    const uint8_t imm8 = imm_byte();
    if (!regFile.get_flag(Flag::Z)) {
      alu_jr(imm8);
      std::println("JR NZ, 0x{:02X}", imm8);
    } else {
      std::println("JR NZ, 0x{:02X} (not taken)", imm8);
    }
    break;
  }
  case 0x30: {
    const uint8_t imm8 = imm_byte();
    if (!regFile.get_flag(Flag::C)) {
      alu_jr(imm8);
      std::println("JR NC, 0x{:02X}", imm8);
    } else {
      std::println("JR NC, 0x{:02X} (not taken)", imm8);
    }
    break;
  }
  case 0x28: {
    const uint8_t imm8 = imm_byte();
    if (regFile.get_flag(Flag::Z)) {
      alu_jr(imm8);
      std::println("JR Z, 0x{:02X}", imm8);
    } else {
      std::println("JR Z, 0x{:02X} (not taken)", imm8);
    }
    break;
  }
  case 0x38: {
    const uint8_t imm8 = imm_byte();
    if (regFile.get_flag(Flag::C)) {
      alu_jr(imm8);
      std::println("JR C, 0x{:02X}", imm8);
    } else {
      std::println("JR C, 0x{:02X} (not taken)", imm8);
    }
    break;
  }
  default:
    std::println(stderr,
                 "Error: Unknown opcode found (PC: 0x{:04X} OPCODE: 0x{:02X})",
                 regFile.pc - 1, byte0);
    break;
  }
}

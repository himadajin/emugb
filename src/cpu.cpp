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

// JR: Jump Relative
void CPU::alu_jr(uint8_t imm8) {
  const int32_t offset = static_cast<int8_t>(imm8);
  regFile.pc = static_cast<uint16_t>(static_cast<int32_t>(regFile.pc) + offset);
}

// ADD A, r8
uint8_t CPU::alu_add(uint8_t imm8) {
  const uint16_t result =
      static_cast<uint16_t>(regFile.a) + static_cast<uint16_t>(imm8);
  regFile.set_flag(Flag::Z, (result & 0xFF) == 0);
  regFile.set_flag(Flag::N, false);
  regFile.set_flag(Flag::H, ((regFile.a & 0x0F) + (imm8 & 0x0F)) > 0x0F);
  regFile.set_flag(Flag::C, result > 0xFF);
  return static_cast<uint8_t>(result);
}

// ADC A, r8
uint8_t CPU::alu_adc(uint8_t imm8) {
  const uint16_t carry = regFile.get_flag(Flag::C) ? 1 : 0;
  const uint16_t result = static_cast<uint16_t>(regFile.a) +
                          static_cast<uint16_t>(imm8) +
                          static_cast<uint16_t>(carry);
  regFile.set_flag(Flag::Z, (result & 0xFF) == 0);
  regFile.set_flag(Flag::N, false);
  regFile.set_flag(Flag::H,
                   ((regFile.a & 0x0F) + (imm8 & 0x0F) + carry) > 0x0F);
  regFile.set_flag(Flag::C, result > 0xFF);
  return static_cast<uint8_t>(result & 0xFF);
}

// SUB A, r8
uint8_t CPU::alu_sub(uint8_t imm8) {
  const uint16_t result =
      static_cast<uint16_t>(regFile.a) - static_cast<uint16_t>(imm8);
  regFile.set_flag(Flag::Z, (result & 0xFF) == 0);
  regFile.set_flag(Flag::N, true);
  regFile.set_flag(Flag::H, (regFile.a & 0x0F) < (imm8 & 0x0F));
  regFile.set_flag(Flag::C, result > 0xFF);
  return static_cast<uint8_t>(result & 0xFF);
}

// SBC A, r8
uint8_t CPU::alu_sbc(uint8_t imm8) {
  const uint16_t carry = regFile.get_flag(Flag::C) ? 1 : 0;
  const uint16_t result =
      static_cast<uint16_t>(regFile.a) - static_cast<uint16_t>(imm8) - carry;
  regFile.set_flag(Flag::Z, (result & 0xFF) == 0);
  regFile.set_flag(Flag::N, true);
  regFile.set_flag(Flag::H, (regFile.a & 0x0F) < ((imm8 & 0x0F) + carry));
  regFile.set_flag(Flag::C, result > 0xFF);
  return static_cast<uint8_t>(result & 0xFF);
}

// AND A, r8
uint8_t CPU::alu_and(uint8_t imm8) {
  const uint8_t result = regFile.a & imm8;
  regFile.set_flag(Flag::Z, result == 0);
  regFile.set_flag(Flag::N, false);
  regFile.set_flag(Flag::H, true);
  regFile.set_flag(Flag::C, false);
  return result;
}

// XOR A, r8
uint8_t CPU::alu_xor(uint8_t imm8) {
  const uint8_t result = regFile.a ^ imm8;
  regFile.set_flag(Flag::Z, result == 0);
  regFile.set_flag(Flag::N, false);
  regFile.set_flag(Flag::H, false);
  regFile.set_flag(Flag::C, false);
  return result;
}

void CPU::execute() {
  const uint8_t byte0 = imm_byte();
  switch (byte0) {
  // NOP
  case 0x00: {
    std::println("NOP");
    break;
  }

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

  // STOP
  case 0x10: {
    std::println("STOP");
    break;
  }

  // LD r8, r8
  case 0x40: {
    std::println("LD B, B");
    break;
  }
  case 0x50: {
    regFile.d = regFile.b;
    std::println("LD D, B");
    break;
  }
  case 0x60: {
    regFile.h = regFile.b;
    std::println("LD H, B");
    break;
  }
  case 0x70: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.b);
    std::println("LD (HL), B");
    break;
  }
  case 0x41: {
    regFile.b = regFile.c;
    std::println("LD B, C");
    break;
  }
  case 0x51: {
    regFile.d = regFile.c;
    std::println("LD D, C");
    break;
  }
  case 0x61: {
    regFile.h = regFile.c;
    std::println("LD H, C");
    break;
  }
  case 0x71: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.c);
    std::println("LD (HL), C");
    break;
  }
  case 0x42: {
    regFile.b = regFile.d;
    std::println("LD B, D");
    break;
  }
  case 0x52: {
    std::println("LD D, D");
    break;
  }
  case 0x62: {
    regFile.h = regFile.d;
    std::println("LD H, D");
    break;
  }
  case 0x72: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.d);
    std::println("LD (HL), D");
    break;
  }
  case 0x43: {
    regFile.b = regFile.e;
    std::println("LD B, E");
    break;
  }
  case 0x53: {
    regFile.d = regFile.e;
    std::println("LD D, E");
    break;
  }
  case 0x63: {
    regFile.h = regFile.e;
    std::println("LD H, E");
    break;
  }
  case 0x73: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.e);
    std::println("LD (HL), E");
    break;
  }
  case 0x44: {
    regFile.b = regFile.h;
    std::println("LD B, H");
    break;
  }
  case 0x54: {
    regFile.d = regFile.h;
    std::println("LD D, H");
    break;
  }
  case 0x64: {
    std::println("LD H, H");
    break;
  }
  case 0x74: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.h);
    std::println("LD (HL), H");
    break;
  }
  case 0x45: {
    regFile.b = regFile.l;
    std::println("LD B, L");
    break;
  }
  case 0x55: {
    regFile.d = regFile.l;
    std::println("LD D, L");
    break;
  }
  case 0x65: {
    regFile.h = regFile.l;
    std::println("LD H, L");
    break;
  }
  case 0x75: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.l);
    std::println("LD (HL), L");
    break;
  }
  case 0x46: {
    const uint16_t addr = regFile.get_hl();
    regFile.b = memory.get_byte(addr);
    std::println("LD B, (HL)");
    break;
  }
  case 0x56: {
    const uint16_t addr = regFile.get_hl();
    regFile.d = memory.get_byte(addr);
    std::println("LD D, (HL)");
    break;
  }
  case 0x66: {
    const uint16_t addr = regFile.get_hl();
    regFile.h = memory.get_byte(addr);
    std::println("LD H, (HL)");
    break;
  }
  case 0x76: {
    std::println("HALT");
    break;
  }
  case 0x47: {
    regFile.b = regFile.a;
    std::println("LD B, A");
    break;
  }
  case 0x57: {
    regFile.d = regFile.a;
    std::println("LD D, A");
    break;
  }
  case 0x67: {
    regFile.h = regFile.a;
    std::println("LD H, A");
    break;
  }
  case 0x77: {
    const uint16_t addr = regFile.get_hl();
    memory.set_byte(addr, regFile.a);
    std::println("LD (HL), A");
    break;
  }
  case 0x48: {
    regFile.c = regFile.b;
    std::println("LD C, B");
    break;
  }
  case 0x58: {
    regFile.e = regFile.b;
    std::println("LD E, B");
    break;
  }
  case 0x68: {
    regFile.l = regFile.b;
    std::println("LD L, B");
    break;
  }
  case 0x78: {
    regFile.a = regFile.b;
    std::println("LD A, B");
    break;
  }
  case 0x49: {
    std::println("LD C, C");
    break;
  }
  case 0x59: {
    regFile.e = regFile.c;
    std::println("LD E, C");
    break;
  }
  case 0x69: {
    regFile.l = regFile.c;
    std::println("LD L, C");
    break;
  }
  case 0x79: {
    regFile.a = regFile.c;
    std::println("LD A, C");
    break;
  }
  case 0x4A: {
    regFile.c = regFile.d;
    std::println("LD C, D");
    break;
  }
  case 0x5A: {
    regFile.e = regFile.d;
    std::println("LD E, D");
    break;
  }
  case 0x6A: {
    regFile.l = regFile.d;
    std::println("LD L, D");
    break;
  }
  case 0x7A: {
    regFile.a = regFile.d;
    std::println("LD A, D");
    break;
  }
  case 0x4B: {
    regFile.c = regFile.e;
    std::println("LD C, E");
    break;
  }
  case 0x5B: {
    regFile.e = regFile.e;
    std::println("LD E, E");
    break;
  }
  case 0x6B: {
    regFile.l = regFile.e;
    std::println("LD L, E");
    break;
  }
  case 0x7B: {
    regFile.a = regFile.e;
    std::println("LD A, E");
    break;
  }
  case 0x4C: {
    regFile.c = regFile.h;
    std::println("LD C, H");
    break;
  }
  case 0x5C: {
    regFile.e = regFile.h;
    std::println("LD E, H");
    break;
  }
  case 0x6C: {
    regFile.l = regFile.h;
    std::println("LD L, H");
    break;
  }
  case 0x7C: {
    regFile.a = regFile.h;
    std::println("LD A, H");
    break;
  }
  case 0x4D: {
    regFile.c = regFile.l;
    std::println("LD C, L");
    break;
  }
  case 0x5D: {
    regFile.e = regFile.l;
    std::println("LD E, L");
    break;
  }
  case 0x6D: {
    regFile.l = regFile.l;
    std::println("LD L, L");
    break;
  }
  case 0x7D: {
    regFile.a = regFile.l;
    std::println("LD A, L");
    break;
  }
  case 0x4E: {
    const uint16_t addr = regFile.get_hl();
    regFile.c = memory.get_byte(addr);
    std::println("LD C, (HL)");
    break;
  }
  case 0x5E: {
    const uint16_t addr = regFile.get_hl();
    regFile.e = memory.get_byte(addr);
    std::println("LD E, (HL)");
    break;
  }
  case 0x6E: {
    const uint16_t addr = regFile.get_hl();
    regFile.l = memory.get_byte(addr);
    std::println("LD L, (HL)");
    break;
  }
  case 0x7E: {
    const uint16_t addr = regFile.get_hl();
    regFile.a = memory.get_byte(addr);
    std::println("LD A, (HL)");
    break;
  }
  case 0x4F: {
    regFile.c = regFile.a;
    std::println("LD C, A");
    break;
  }
  case 0x5F: {
    regFile.e = regFile.a;
    std::println("LD E, A");
    break;
  }
  case 0x6F: {
    regFile.l = regFile.a;
    std::println("LD L, A");
    break;
  }
  case 0x7F: {
    regFile.a = regFile.a;
    std::println("LD A, A");
    break;
  }

  // ADD A, r8
  case 0x80: {
    regFile.a = alu_add(regFile.b);
    std::println("ADD A, B");
    break;
  }
  case 0x81: {
    regFile.a = alu_add(regFile.c);
    std::println("ADD A, C");
    break;
  }
  case 0x82: {
    regFile.a = alu_add(regFile.d);
    std::println("ADD A, D");
    break;
  }
  case 0x83: {
    regFile.a = alu_add(regFile.e);
    std::println("ADD A, E");
    break;
  }
  case 0x84: {
    regFile.a = alu_add(regFile.h);
    std::println("ADD A, H");
    break;
  }
  case 0x85: {
    regFile.a = alu_add(regFile.l);
    std::println("ADD A, L");
    break;
  }
  case 0x86: {
    const uint16_t addr = regFile.get_hl();
    const uint8_t value = memory.get_byte(addr);
    regFile.a = alu_add(value);
    std::println("ADD A, (HL)");
    break;
  }
  case 0x87: {
    regFile.a = alu_add(regFile.a);
    std::println("ADD A, A");
    break;
  }

  // ADC A, r8
  case 0x88: {
    regFile.a = alu_adc(regFile.b);
    std::println("ADC A, B");
    break;
  }
  case 0x89: {
    regFile.a = alu_adc(regFile.c);
    std::println("ADC A, C");
    break;
  }
  case 0x8A: {
    regFile.a = alu_adc(regFile.d);
    std::println("ADC A, D");
    break;
  }
  case 0x8B: {
    regFile.a = alu_adc(regFile.e);
    std::println("ADC A, E");
    break;
  }
  case 0x8C: {
    regFile.a = alu_adc(regFile.h);
    std::println("ADC A, H");
    break;
  }
  case 0x8D: {
    regFile.a = alu_adc(regFile.l);
    std::println("ADC A, L");
    break;
  }
  case 0x8E: {
    const uint16_t addr = regFile.get_hl();
    const uint8_t value = memory.get_byte(addr);
    regFile.a = alu_adc(value);
    std::println("ADC A, (HL)");
    break;
  }
  case 0x8F: {
    regFile.a = alu_adc(regFile.a);
    std::println("ADC A, A");
    break;
  }

  // SUB A, r8
  case 0x90: {
    regFile.a = alu_sub(regFile.b);
    std::println("SUB A, B");
    break;
  }
  case 0x91: {
    regFile.a = alu_sub(regFile.c);
    std::println("SUB A, C");
    break;
  }
  case 0x92: {
    regFile.a = alu_sub(regFile.d);
    std::println("SUB A, D");
    break;
  }
  case 0x93: {
    regFile.a = alu_sub(regFile.e);
    std::println("SUB A, E");
    break;
  }
  case 0x94: {
    regFile.a = alu_sub(regFile.h);
    std::println("SUB A, H");
    break;
  }
  case 0x95: {
    regFile.a = alu_sub(regFile.l);
    std::println("SUB A, L");
    break;
  }
  case 0x96: {
    const uint16_t addr = regFile.get_hl();
    const uint8_t value = memory.get_byte(addr);
    regFile.a = alu_sub(value);
    std::println("SUB A, (HL)");
    break;
  }
  case 0x97: {
    regFile.a = alu_sub(regFile.a);
    std::println("SUB A, A");
    break;
  }

  // SBC A, r8
  case 0x98: {
    regFile.a = alu_sbc(regFile.b);
    std::println("SBC A, B");
    break;
  }
  case 0x99: {
    regFile.a = alu_sbc(regFile.c);
    std::println("SBC A, C");
    break;
  }
  case 0x9A: {
    regFile.a = alu_sbc(regFile.d);
    std::println("SBC A, D");
    break;
  }
  case 0x9B: {
    regFile.a = alu_sbc(regFile.e);
    std::println("SBC A, E");
    break;
  }
  case 0x9C: {
    regFile.a = alu_sbc(regFile.h);
    std::println("SBC A, H");
    break;
  }
  case 0x9D: {
    regFile.a = alu_sbc(regFile.l);
    std::println("SBC A, L");
    break;
  }
  case 0x9E: {
    const uint16_t addr = regFile.get_hl();
    const uint8_t value = memory.get_byte(addr);
    regFile.a = alu_sbc(value);
    std::println("SBC A, (HL)");
    break;
  }
  case 0x9F: {
    regFile.a = alu_sbc(regFile.a);
    std::println("SBC A, A");
    break;
  }

  // AND A, r8
  case 0xA0: {
    regFile.a = alu_and(regFile.b);
    std::println("AND A, B");
    break;
  }
  case 0xA1: {
    regFile.a = alu_and(regFile.c);
    std::println("AND A, C");
    break;
  }
  case 0xA2: {
    regFile.a = alu_and(regFile.d);
    std::println("AND A, D");
    break;
  }
  case 0xA3: {
    regFile.a = alu_and(regFile.e);
    std::println("AND A, E");
    break;
  }
  case 0xA4: {
    regFile.a = alu_and(regFile.h);
    std::println("AND A, H");
    break;
  }
  case 0xA5: {
    regFile.a = alu_and(regFile.l);
    std::println("AND A, L");
    break;
  }
  case 0xA6: {
    const uint16_t addr = regFile.get_hl();
    const uint8_t value = memory.get_byte(addr);
    regFile.a = alu_and(value);
    std::println("AND A, (HL)");
    break;
  }
  case 0xA7: {
    regFile.a = alu_and(regFile.a);
    std::println("AND A, A");
    break;
  }

  case 0xA8: {
    regFile.a = alu_xor(regFile.b);
    std::println("XOR A, B");
    break;
  }
  case 0xA9: {
    regFile.a = alu_xor(regFile.c);
    std::println("XOR A, C");
    break;
  }
  case 0xAA: {
    regFile.a = alu_xor(regFile.d);
    std::println("XOR A, D");
    break;
  }
  case 0xAB: {
    regFile.a = alu_xor(regFile.e);;
    std::println("XOR A, E");
    break;
  }
  case 0xAC: {
    regFile.a = alu_xor(regFile.h);
    std::println("XOR A, H");
    break;
  }
  case 0xAD: {
    regFile.a = alu_xor(regFile.l);
    std::println("XOR A, L");
    break;
  }
  case 0xAE: {
    const uint16_t addr = regFile.get_hl();
    const uint8_t value = memory.get_byte(addr);
    regFile.a = alu_xor(value);
    std::println("XOR A, (HL)");
    break;
  }
  case 0xAF: {
    regFile.a = alu_xor(regFile.a);
    std::println("XOR A, A");
    break;
  }
  default: {
    std::println(stderr,
                 "Error: Unknown opcode found (PC: 0x{:04X} OPCODE: 0x{:02X})",
                 regFile.pc - 1, byte0);
    break;
  }
  }
}

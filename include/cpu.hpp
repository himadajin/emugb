#ifndef EMUGB_INCLUDE_CPU_HPP
#define EMUGB_INCLUDE_CPU_HPP

#include "memory.hpp"
#include "register.hpp"

class CPU {
public:
  RegFile regFile;
  Memory &memory;

  CPU(Memory &memory) : regFile(), memory(memory) {}

  uint8_t imm_byte();
  uint16_t imm_word();

  void alu_add_hl(uint16_t imm16);
  uint8_t alu_inc(uint8_t imm8);
  uint8_t alu_dec(uint8_t imm8);
  void alu_jr(uint8_t imm8);
  uint8_t alu_add(uint8_t imm8);
  uint8_t alu_adc(uint8_t imm8);
  uint8_t alu_sub(uint8_t imm8);
  uint8_t alu_sbc(uint8_t imm8);
  uint8_t alu_and(uint8_t imm8);

  void execute();
};

#endif // EMUGB_INCLUDE_CPU_HPP

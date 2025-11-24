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

  void execute();
};

#endif // EMUGB_INCLUDE_CPU_HPP

#ifndef EMUGB_INCLUDE_MEMORY_HPP
#define EMUGB_INCLUDE_MEMORY_HPP

#include <cstdint>

class Memory {
public:
  virtual uint8_t get_byte(uint16_t addr) const = 0;
  virtual void set_byte(uint16_t addr, uint8_t value) = 0;

  uint16_t get_word(uint16_t addr) const;
  void set_word(uint16_t addr, uint16_t value);
};
#endif // EMUGB_INCLUDE_MEMORY_HPP

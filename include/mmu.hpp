#ifndef EMUGB_INCLUDE_MMU_HPP
#define EMUGB_INCLUDE_MMU_HPP

#include "cartridge.hpp"
#include "memory.hpp"
#include <memory>

class MMU : public Memory {
private:
  std::unique_ptr<Cartridge> cartridge;

public:
  MMU(std::unique_ptr<Cartridge> cartridge) : cartridge(std::move(cartridge)) {}

  uint8_t get_byte(uint16_t addr) const override;
  void set_byte(uint16_t addr, uint8_t value) override;
};

#endif // EMUGB_INCLUDE_MMU_HPP

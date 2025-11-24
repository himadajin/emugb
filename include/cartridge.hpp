#ifndef EMUGB_INCLUDE_CARTRIDGE_HPP
#define EMUGB_INCLUDE_CARTRIDGE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "memory.hpp"

class Cartridge : public Memory {
public:
  virtual ~Cartridge() = default;
  virtual uint8_t get_byte(uint16_t addr) const = 0;
  virtual void set_byte(uint16_t addr, uint8_t value) = 0;

  std::string get_title() const;
};

class RomOnly : public Cartridge {
public:
  RomOnly(std::vector<uint8_t> &&rom) : rom(std::move(rom)) {}
  ~RomOnly() = default;

  uint8_t get_byte(uint16_t addr) const override;
  void set_byte(uint16_t addr, uint8_t value) override;

  const std::vector<uint8_t> &get_rom() const { return rom; }

private:
  std::vector<uint8_t> rom;
};

std::unique_ptr<Cartridge> load_from_path(const std::string &path);

#endif // EMUGB_INCLUDE_CARTRIDGE_HPP

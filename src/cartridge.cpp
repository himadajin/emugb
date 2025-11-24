#include "cartridge.hpp"

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <print>
#include <sstream>
#include <string>
#include <vector>

uint8_t RomOnly::get_byte(uint16_t addr) const {
  if (addr >= rom.size()) {
    return 0;
  }
  return rom[addr];
}

// ROM is read-only; writes are intentionally ignored.
void RomOnly::set_byte(uint16_t addr, uint8_t value) {}

std::string Cartridge::get_title() const {
  std::stringstream ss;
  const size_t title_start = 0x0134;
  const size_t title_end = 0x0143;

  for (size_t addr = title_start; addr <= title_end; ++addr) {
    const uint8_t byte = get_byte(static_cast<uint16_t>(addr));
    if (byte == 0) {
      break;
    }
    ss << static_cast<char>(byte);
  }

  return ss.str();
}

std::unique_ptr<Cartridge> load_from_path(const std::string &path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::println(stderr, "Error: Could not open file {}", path);
    std::exit(EXIT_FAILURE);
  }

  std::vector<uint8_t> rom((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
  file.close();

  return std::make_unique<RomOnly>(std::move(rom));
}

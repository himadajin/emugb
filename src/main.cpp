#include "cartridge.hpp"
#include "mmu.hpp"
#include <memory>
#include <print>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::println(stderr, "Usage: {} <rom_path>", argv[0]);
    return 1;
  }

  const char *rom_path = argv[1];
  std::unique_ptr<Cartridge> cartridge = load_from_path(rom_path);
  std::println("title: {}", cartridge->get_title());
  MMU mmu(std::move(cartridge));
  for (size_t addr = 0x0150; addr <= 0x01FF; ++addr) {
    std::println("Byte at 0x{:04X}: {:02X}", addr,
                 mmu.get_byte(static_cast<uint16_t>(addr)));
  }
  return 0;
}

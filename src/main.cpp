#include "cartridge.hpp"
#include "cpu.hpp"
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
  CPU cpu(mmu);
  cpu.regFile.pc = 0x150;
  for (size_t i = 0; i < 10; ++i) {
    cpu.execute();
  }
  return 0;
}

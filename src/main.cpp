#include "cartridge.hpp"
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
  return 0;
}

#ifndef EMUGB_INCLUDE_REGISTER_HPP
#define EMUGB_INCLUDE_REGISTER_HPP

#include <cstdint>
#include <format>

enum class Flag : uint8_t {
  Z = 0b10000000, // Zero Flag
  N = 0b01000000, // Subtract Flag
  H = 0b00100000, // Half Carry Flag
  C = 0b00010000  // Carry Flag
};

class RegFile {
public:
  uint8_t a;
  uint8_t f;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  uint16_t sp;
  uint16_t pc;

  RegFile() : a(0), f(0), b(0), c(0), d(0), e(0), h(0), l(0), sp(0), pc(0) {}

  inline uint16_t get_af() const {
    const uint16_t af = (static_cast<uint16_t>(a) << 8) | f;
    return af;
  }

  inline uint16_t get_bc() const {
    const uint16_t bc = (static_cast<uint16_t>(b) << 8) | c;
    return bc;
  }

  inline uint16_t get_de() const {
    const uint16_t de = (static_cast<uint16_t>(d) << 8) | e;
    return de;
  }

  inline uint16_t get_hl() const {
    const uint16_t hl = (static_cast<uint16_t>(h) << 8) | l;
    return hl;
  }

  inline void set_af(uint16_t af) {
    a = (af >> 8) & 0xFF;
    f = af & 0xFF;
  }

  inline void set_bc(uint16_t bc) {
    b = (bc >> 8) & 0xFF;
    c = bc & 0xFF;
  }

  inline void set_de(uint16_t de) {
    d = (de >> 8) & 0xFF;
    e = de & 0xFF;
  }

  inline void set_hl(uint16_t hl) {
    h = (hl >> 8) & 0xFF;
    l = hl & 0xFF;
  }

  inline bool get_flag(Flag flag) const {
    return (f & static_cast<uint8_t>(flag)) != 0;
  }

  inline void set_flag(Flag flag, bool value) {
    if (value) {
      // set only the bit corresponding to flag to 1
      f |= static_cast<uint8_t>(flag);
    } else {
      // set only the bit corresponding to flag to 0
      f &= ~static_cast<uint8_t>(flag);
    }
  }
};

template <> struct std::formatter<RegFile> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const RegFile &regfile, FormatContext &ctx) const {
    return std::format_to(
        ctx.out(),
        "AF: {:04X} BC: {:04X} DE: {:04X} HL: {:04X} SP: {:04X} PC: {:04X}",
        regfile.get_af(), regfile.get_bc(), regfile.get_de(), regfile.get_hl(),
        regfile.sp, regfile.pc);
  }
};

#endif // EMUGB_INCLUDE_REGISTER_HPP

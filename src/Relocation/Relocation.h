#pragma once
#include <stdint.h>
#include <string>

struct Relocation {
  uint32_t address;
  uint8_t opCode;
  std::string symbolName;
};
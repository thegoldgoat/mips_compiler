#pragma once
#include <stdint.h>

struct Relocation {
  uint32_t address;
  uint8_t opCode;
  char *symbolName;
};
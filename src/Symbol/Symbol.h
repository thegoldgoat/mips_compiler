#pragma once
#include <stdint.h>
#include <string.h>

enum SymbolType { SYMBOL_TEXT, SYMBOL_DATA };

struct Symbol {
  char *name;
  SymbolType type;
  uint16_t address;
};
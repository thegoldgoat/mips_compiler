#pragma once
#include <stdint.h>
#include <string>

enum SymbolType { SYMBOL_TEXT, SYMBOL_DATA, SYMBOL_NONE };

struct Symbol {
  std::string name;
  SymbolType type;
  uint16_t address;
  bool isGlobal;
};
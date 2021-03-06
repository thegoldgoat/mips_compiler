#pragma once
#include <stdint.h>
#include <string>

enum SymbolType : uint8_t { SYMBOL_TEXT, SYMBOL_DATA, SYMBOL_NONE };

struct Symbol {
  std::string name;
  SymbolType type;
  uint32_t address;
  bool isGlobal;
};

struct SymbolForMap {
  SymbolType type;
  uint32_t address;
};

bool isValidSymbolName(std::string &symbolName);
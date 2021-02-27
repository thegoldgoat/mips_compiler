#pragma once
#include <stdint.h>
#include <string>

enum SymbolType : uint8_t { SYMBOL_TEXT, SYMBOL_DATA, SYMBOL_NONE };

struct Symbol {
  std::string name;
  SymbolType type;
  uint16_t address;
  bool isGlobal;
};

bool isValidSymbolName(std::string &symbolName);
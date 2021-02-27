#include "Symbol.hpp"

bool isValidSymbolName(std::string &symbolName) {

  // Cannot start with a digit
  if (symbolName[0] >= '0' && symbolName[0] <= '9')
    return false;

  for (auto iterator : symbolName)
    if (iterator != '_' && iterator < 'a' && iterator > 'z' && iterator < 'A' &&
        iterator > 'Z' && iterator < '0' && iterator > '9')
      return false;

  return true;
}
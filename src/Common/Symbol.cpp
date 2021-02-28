#include "Symbol.hpp"

#include <iostream>

#define VALUE_BETWEEN(value, upperBound, lowerBound)                           \
  (value <= upperBound && value >= lowerBound)

bool isValidSymbolName(std::string &symbolName) {

  if (symbolName.size() == 0)
    return false;

  // Cannot start with a digit
  if (symbolName[0] >= '0' && symbolName[0] <= '9')
    return false;

  for (auto iterator : symbolName)
    if (iterator != '_' && !VALUE_BETWEEN(iterator, 'z', 'a') &&
        !VALUE_BETWEEN(iterator, 'Z', 'A') &&
        !VALUE_BETWEEN(iterator, '9', '0'))
      return false;

  return true;
}
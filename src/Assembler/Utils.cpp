#include "Utils.h"

void makeSymbolGlobal(std::vector<Symbol> &symbolTable,
                      std::string &symbolName) {

  // Check if symbol is already in
  for (auto &iterator : symbolTable) {
    if (iterator.name == symbolName) {
      iterator.isGlobal = true;
      return;
    }
  }

  // If not found, add a new symbol with that name
  symbolTable.push_back({symbolName, SYMBOL_NONE, 0, true});
}
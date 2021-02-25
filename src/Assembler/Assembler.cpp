#include "Assembler.h"
#include <iostream>
#include <sstream>

Assembler::Assembler(std::ifstream *fileContent) : fileStream(fileContent) {}

FileObject Assembler::assemble() {

  std::string stringBuffer;

  FileObject returnValue;

  while (std::getline(*fileStream, stringBuffer))
    parseLine(stringBuffer);

  return returnValue;
}

void Assembler::makeSymbolGlobal(std::string &symbolName) {

  // Check if symbol is already in
  for (auto &iterator : returnValue.symbolTable) {
    if (iterator.name == symbolName) {
      iterator.isGlobal = true;
      return;
    }
  }

  // If not found, add a new symbol with that name
  returnValue.symbolTable.push_back({symbolName, SYMBOL_NONE, 0, true});
}

std::string Assembler::removeBeginningSpaces(std::string input) {
  uint32_t endSpaceIndex = 0;

  for (auto iterator : input) {
    if (iterator != ' ')
      break;
    endSpaceIndex++;
  }

  return input.substr(endSpaceIndex);
}

void Assembler::parseLine(std::string line) {
  std::string stringBuffer;
  line = removeBeginningSpaces(line);
  std::cout << "Parsing " << line << std::endl;

  if (line[0] == ';') {
    return;
  } else if (line[0] == '.') {
    // Section or other special keywords
    std::stringstream noSpaceBuffer(line);
    noSpaceBuffer >> stringBuffer;

    std::cout << "[.]: Special word: " << stringBuffer << std::endl;

    if (stringBuffer == ".text") {
      currentSection = TEXT;

    } else if (stringBuffer == ".data") {
      currentSection = DATA;
    } else if (stringBuffer == ".globl") {
      // Read symbol name
      noSpaceBuffer >> stringBuffer;

      std::cout << "[.globl]: exporting symbol named " << stringBuffer
                << std::endl;

      makeSymbolGlobal(stringBuffer);
    }

  } else {
    // Read
  }
}
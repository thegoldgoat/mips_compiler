#include "Assembler.h"
#include <assert.h>
#include <iostream>
#include <sstream>

Assembler::Assembler(std::ifstream *fileContent) : fileStream(fileContent) {}

FileObject Assembler::assemble() {

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

std::string Assembler::removeBeginningSpaces(std::string &input) {
  uint32_t endSpaceIndex = 0;

  for (auto iterator : input) {
    if (iterator != ' ')
      break;
    endSpaceIndex++;
  }

  return input.substr(endSpaceIndex);
}

void Assembler::parseLine(std::string &line) {
  line = removeBeginningSpaces(line);
  std::cout << "Parsing " << line << std::endl;

  if (line[0] == ';') {
    return;
  } else if (line[0] == '.') {
    // Section or other special keywords
    dotLine(line);
  } else {
    instructionLine(line);
  }
}

void Assembler::dotLine(std::string &line) {
  std::stringstream stringStream(line);
  stringStream >> stringBuffer;

  std::cout << "[.]: Special word: " << stringBuffer << std::endl;

  if (stringBuffer == ".text") {
    currentSection = TEXT;

  } else if (stringBuffer == ".data") {
    currentSection = DATA;
  } else if (stringBuffer == ".globl") {
    // Read symbol name
    stringStream >> stringBuffer;

    std::cout << "[.globl]: exporting symbol named " << stringBuffer
              << std::endl;

    makeSymbolGlobal(stringBuffer);
  }
}

#define LABEL_IN_NONE_SECTION_MESSAGE "[ERROR]: Label in 'NONE' section"

void Assembler::instructionLine(std::string &line) {
  // Read First word and check if it is a label
  std::stringstream stringStream(line);
  stringStream >> stringBuffer;

  if (stringBuffer.back() == ':') {
    stringBuffer.pop_back();
    std::cout << "[label]: name = " << stringBuffer << std::endl;
    switch (currentSection) {
    case NONE:
      std::cerr << LABEL_IN_NONE_SECTION_MESSAGE << std::endl;
      throw std::runtime_error(LABEL_IN_NONE_SECTION_MESSAGE);
      break;
    case DATA:
      // Parse variable type
      stringStream >> stringBuffer;

      if (stringBuffer == ".word") {
        parseWord(stringStream);
      } else if (stringBuffer == ".space") {
        parseSpace(stringStream);
      }
      break;
    case TEXT:
      parseInstruction(stringStream);
      currentTextSize += 4;
      break;
    }
  }
}

void Assembler::parseWord(std::stringstream &ss) {
  assert(currentSection == DATA);
}

void Assembler::parseSpace(std::stringstream &ss) {
  assert(currentSection == DATA);
  uint32_t spaceSize;
  ss >> spaceSize;
  std::cout << "[.space] of size " << spaceSize << std::endl;
  currentDataSize += spaceSize;
}

void Assembler::parseInstruction(std::stringstream &ss) {
  assert(currentSection == TEXT);
}

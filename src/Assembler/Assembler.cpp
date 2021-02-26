#include "Assembler.h"
#include "Utils.h"

#include <assert.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>

Assembler::Assembler(std::ifstream *fileContent) : fileStream(fileContent) {}

FileObject Assembler::assemble() {
  int lineNumber = 1;
  while (std::getline(*fileStream, stringBuffer)) {
    try {
      parseLine(stringBuffer);
    } catch (std::runtime_error &error) {
      std::cerr << std::endl
                << "[ASSEMBLER ERROR]: Line number " << lineNumber << std::endl
                << "Error message: " << error.what() << std::endl
                << std::endl;
      exit(-1);
    }
    lineNumber++;
  }

  return returnValue;
}

#define MAX_SYMBOL_TABLE_SIZE 65536

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

void Assembler::addNewSymbol(std::string name, SymbolType type) {
  // TODO: Fix algorith looks like this target pointer method doesn't work with
  // the vector since the value pushed in the vector is maybe a deep copy (?)

  assert(type != SYMBOL_NONE);

  std::cout << "[Symbol]: name = " << name << " type = " << type << std::endl;

  // Check if symbol is already in
  Symbol *target = NULL;
  for (auto &iterator : returnValue.symbolTable) {
    if (iterator.name == name) {
      if (iterator.type == SYMBOL_NONE) {
        // It was previously exported
        target = &iterator;
        break;
      } else {
        throw std::runtime_error(
            std::string("Trying to add symbol ") + name +
            std::string("while already present (and not just exported)."));
      }
    }
  }

  if (!target) {
    Symbol newSymbol;
    newSymbol.isGlobal = false;
    returnValue.symbolTable.push_back(newSymbol);
    target = &newSymbol;
  }

  // Add symbol
  target->name = name;
  target->type = type;
  target->address = returnValue.symbolTable.size() * 4;
}

std::string Assembler::removeBeginningSpacesAndComment(std::string &input) {
  uint32_t endSpaceIndex = 0;
  int32_t commentIndex = -1;
  uint32_t spaceBeforeCommentCount = 0;

  for (auto iterator : input) {
    if (iterator != ' ')
      break;
    endSpaceIndex++;
  }

  int inputSize = input.size();

  for (int i = endSpaceIndex; i < inputSize; i++) {
    if (input[i] == ';') {
      commentIndex = i;
      for (i--; i >= 0; i--) {
        if (input[i] != ' ')
          break;
        spaceBeforeCommentCount++;
      }
      break;
    }
  }

  if (commentIndex == -1)
    return input.substr(endSpaceIndex);
  else
    return input.substr(endSpaceIndex,
                        commentIndex - spaceBeforeCommentCount - endSpaceIndex);
}

void Assembler::parseLine(std::string &line) {
  line = removeBeginningSpacesAndComment(line);
  std::cout << "Parsing " << line << std::endl;

  if (line[0] == '.') {
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

  if (stringBuffer.size() == 0)
    return;

  if (stringBuffer.back() == ':') {
    stringBuffer.pop_back();
    switch (currentSection) {
    case NONE:
      std::cerr << LABEL_IN_NONE_SECTION_MESSAGE << std::endl;
      throw std::runtime_error(LABEL_IN_NONE_SECTION_MESSAGE);
      break;
    case DATA:
      // Register Symbol
      addNewSymbol(stringBuffer, SYMBOL_DATA);

      // Parse variable type
      stringStream >> stringBuffer;

      if (stringBuffer == ".word") {
        parseWord(stringStream);
      } else if (stringBuffer == ".space") {
        parseSpace(stringStream);
      }
      break;
    case TEXT:
      // Register Symbol
      addNewSymbol(stringBuffer, SYMBOL_TEXT);

      if (!stringStream.eof()) {
        // Get word after symbol
        stringStream >> stringBuffer;
        parseInstruction(stringBuffer, stringStream);
      }

      break;
    }
  } else {
    parseInstruction(stringBuffer, stringStream);
  }
}

void Assembler::parseWord(std::stringstream &ss) {
  assert(currentSection == DATA);

  while (std::getline(ss, stringBuffer, ',')) {
    try {
      returnValue.dataSegment.push_back(std::stoi(stringBuffer));
    } catch (std::invalid_argument &) {
      throw std::runtime_error(
          std::string("Error parsing integer after .word: ") + stringBuffer);
    }
  }
}

void Assembler::parseSpace(std::stringstream &ss) {
  assert(currentSection == DATA);
  uint32_t spaceSize;
  ss >> spaceSize;
  std::cout << "[.space] of size " << spaceSize << std::endl;

  int wordsCount = ceil((float)spaceSize / 4);
  for (int i = 0; i < wordsCount; i++)
    returnValue.dataSegment.push_back(0);
}

void Assembler::parseInstruction(std::string instructionCode,
                                 std::stringstream &ss) {
  assert(currentSection == TEXT);

  std::cout << "[DEBUG]: Instruction code: " << instructionCode << std::endl;

  uint32_t totalInstruction = 0;
  InstructionsAvailable instructionEnum = resolveInstruction(instructionCode);

  uint8_t opCode = getOpCode(instructionEnum);
  totalInstruction |= opCode << 26;

  auto operands = getOperands(ss);

  populateInstructionWithOperands(operands, &totalInstruction, instructionEnum);

  if (returnValue.textSegment.size() > MAX_TEXT_SEGMENT_SIZE)
    throw std::runtime_error("Data segment size exceeded");

  returnValue.textSegment.push_back(totalInstruction);
}

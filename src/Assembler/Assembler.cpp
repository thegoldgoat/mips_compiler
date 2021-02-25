#include "Assembler.h"
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
    } catch (std::runtime_error error) {
      std::cerr << std::endl
                << "[ASSEMBLER ERROR]: Line number " << lineNumber << std::endl
                << "Error message: " << error.what() << std::endl
                << std::endl;
      throw -1;
    }
    lineNumber++;
  }

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
      parseInstruction(stringBuffer, stringStream);
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
    } catch (std::invalid_argument) {
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

  // TODO: Parse operands based on instructionCode
}

#include "Assembler.hpp"

#include <assert.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string>

Assembler::Assembler(std::ifstream *fileContent) : fileStream(fileContent) {}

void Assembler::assemble() {
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

  doneAssemblying = true;
}

FileObject Assembler::getObject() {
  if (doneAssemblying)
    return returnValue;

  throw std::runtime_error("Trying to get object before assemblying!");
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
  assert(type != SYMBOL_NONE);

  std::cout << "[Symbol]: name = " << name << " type = " << type << std::endl;

  // Check if symbol is already in
  for (auto &iterator : returnValue.symbolTable) {
    if (iterator.name == name) {
      if (iterator.type == SYMBOL_NONE) {
        // It was previously exported
        iterator.name = name;
        iterator.type = type;
        return;
      } else {
        throw std::runtime_error(
            std::string("Trying to add symbol ") + name +
            std::string("while already present (and not just exported)."));
      }
    }
  }

  // Add symbol
  returnValue.symbolTable.push_back(
      {name, type, static_cast<uint16_t>(returnValue.symbolTable.size() * 4),
       false});
}

std::string Assembler::removeBeginningSpacesAndComment(std::string &input) {
  int32_t endSpaceIndex = 0;
  int32_t commentIndex = -1;
  int32_t spaceBeforeCommentCount = 0;

  for (auto iterator : input) {
    if (iterator != ' ')
      break;
    endSpaceIndex++;
  }

  int inputSize = input.size();

  for (int i = endSpaceIndex; i < inputSize; i++) {
    if (input[i] == ';') {
      commentIndex = i;
      for (i--; i > endSpaceIndex; i--) {
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

    if (!isValidSymbolName(stringBuffer))
      throw std::runtime_error("Invalid symbol name: " + stringBuffer);

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

  populateInstructionWithOperands(operands, &totalInstruction, opCode,
                                  instructionEnum);

  if (returnValue.textSegment.size() > MAX_TEXT_SEGMENT_SIZE)
    throw std::runtime_error("Data segment size exceeded");

  returnValue.textSegment.push_back(totalInstruction);
}

#define SET_FUNCT(target, code) target |= code

void Assembler::populateInstructionWithOperands(
    std::vector<std::string> &operands, uint32_t *instruction, uint8_t &opCode,
    InstructionsAvailable &instructionEnum) {

  try {

    std::pair<std::string, std::string> *offsetAndRegister = NULL;

    switch (instructionEnum) {
    case ADD:
    case ADDU:
    case SUB:
    case SUBU:
    case SLT:
    case SLTU:
    case AND:
    case OR:
    case NOR:
    case XOR:
      // Syntax: Rd, Rs, Rt with order in instruction as Rs, Rt, Rd,
      // funct_code
      *instruction |= getRegisterNumberFromString(operands.at(1)) << 21;
      *instruction |= getRegisterNumberFromString(operands.at(2)) << 16;
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 11;
      break;
    case MULT:
    case MULTU:
    case DIV:
    case DIVU:
      // Syntax: Rs, Rt with order in instruction as Rs, Rt
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 21;
      *instruction |= getRegisterNumberFromString(operands.at(1)) << 16;
      break;
    case ADDI:
    case ADDIU:
    case SLTI:
    case SLTIU:
    case ANDI:
    case ORI:
    case XORI:
    case BEQ:
    case BNE:
      // Syntax: Rt, Rs, Immediate with order in instruction as Rs, Rt,
      // Immediate
      *instruction |= getRegisterNumberFromString(operands.at(1)) << 21;
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 16;
      *instruction |= getImmediateFromString(operands.at(2), opCode, true, 16);
      break;

    case LW:
    case SW:
    case LBU:
    case LB:
    case SB:
      // Syntax: Rt, offset(Rs) with order in memory as Rs, Rt, offset
      offsetAndRegister = getOffsetAndRegister(operands.at(1));

      *instruction |= getRegisterNumberFromString(operands.at(0)) << 16;
      *instruction |= getRegisterNumberFromString(offsetAndRegister->second)
                      << 21;
      *instruction |=
          getImmediateFromString(offsetAndRegister->first, opCode, true, 16);

      delete offsetAndRegister;
      break;
    case LUI:
      // Syntax: Rt, Immediate with order in memory as 00000, rt, Immediate
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 16;
      *instruction |= getImmediateFromString(operands.at(1), opCode, false, 16);
      break;
    case BLEZ:
    case BGTZ:
    case BLTZ:
      // Syntax: Rs, offset with order in memory as rs, 00000, offset
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 21;
      *instruction |= getImmediateFromString(operands.at(1), opCode, false, 16);
      break;
    case J:
    case JAL:
      // Syntax: Immediate with order in memory as Immediate
      *instruction |= getImmediateFromString(operands.at(0), opCode, true, 24);
      break;
    case JALR:
      // Syntax: Rd, Rs with order in memory as rs, 00000, Rd, 00000, 001001
      *instruction |= getRegisterNumberFromString(operands.at(1)) << 21;
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 11;
      break;

    case JR:
      // Syntax: Rs with order in memory as rs, 00000, 00000, 00000, 001000
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 21;
      break;
    case NOP:
      // All zeros
      *instruction = 0;
      break;
    case MFHI:
    case MFLO:
      // Syntax: Rd with order in memory as 00000, 00000, rd,  00000,
      // funct_code
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 11;
      break;
    case SYSCALL_INST:
      // No arguments
      if (operands.size() != 0)
        throw std::runtime_error("syscall must have zero operands.");
      break;
    }

    // Funct code for arithmetic
    switch (instructionEnum) {
    case ADD:
      SET_FUNCT(*instruction, 32);
      break;
    case ADDU:
      SET_FUNCT(*instruction, 33);
      break;
    case SUB:
      SET_FUNCT(*instruction, 34);
      break;
    case SUBU:
      SET_FUNCT(*instruction, 35);
      break;
    case SLT:
      SET_FUNCT(*instruction, 42);
      break;
    case SLTU:
      SET_FUNCT(*instruction, 43);
      break;
    case AND:
      SET_FUNCT(*instruction, 36);
      break;
    case OR:
      SET_FUNCT(*instruction, 37);
      break;
    case NOR:
      SET_FUNCT(*instruction, 39);
      break;
    case XOR:
      SET_FUNCT(*instruction, 40);
      break;
    case MULT:
      SET_FUNCT(*instruction, 24);
      break;
    case MULTU:
      SET_FUNCT(*instruction, 25);
      break;
    case DIV:
      SET_FUNCT(*instruction, 26);
      break;
    case DIVU:
      SET_FUNCT(*instruction, 27);
      break;
    case JR:
      SET_FUNCT(*instruction, 8);
      break;
    case JALR:
      SET_FUNCT(*instruction, 9);
      break;
    case SYSCALL_INST:
      SET_FUNCT(*instruction, 12);
      break;
    default:
      break;
    }
  } catch (std::out_of_range &exception) {
    throw std::runtime_error(std::string("Missing operand: ") +
                             exception.what());
  }
}

uint32_t Assembler::getImmediateFromString(std::string &string, uint8_t &opCode,
                                           bool canBeSymbol,
                                           uint8_t immediateBitSize) {

  // Check if it is a valid Symbol, in that case add it to the relocation table
  // and return 0
  if (isValidSymbolName(string)) {

    if (!canBeSymbol)
      throw std::runtime_error("Immediate invalid: " + string);

    returnValue.relocationTable.push_back(
        {static_cast<uint32_t>(returnValue.textSegment.size() * 4), opCode,
         string});

    return 0;
  } else
    // Just parse the immediate
    try {
      auto result = std::stoi(string);

      // TODO: Detect out of range including negative numbers
      if (result >> immediateBitSize != 0)
        throw std::out_of_range("");

      return result;
    } catch (std::invalid_argument &) {
      throw std::runtime_error("Immediate invalid: " + string);
    } catch (std::out_of_range &) {
      throw std::runtime_error("Immediate out of range: " + string);
    }
}

void Assembler::prettyPrintObject() {

  unsigned int count = 0;

  std::cout << "--------------------" << std::endl
            << "--- Text Segment ---" << std::endl;

  for (auto iterator : returnValue.textSegment)
    printf("[%08x]: %08x\n", (count++) * 4, iterator);

  std::cout << "--------------------" << std::endl
            << "--- Data Segment ---" << std::endl;

  count = 0;
  for (auto iterator : returnValue.dataSegment)
    printf("[%08x]: %08x\n", (count++) * 4, iterator);

  std::cout << "--------------------" << std::endl
            << "--- Symbol Table ---" << std::endl;

  count = 0;
  for (auto iterator : returnValue.symbolTable)
    printf("[%08x]: Name = %s; Address = %08x; Type = %x; isGLobal = %x\n",
           (count++) * 4, iterator.name.c_str(), iterator.address,
           iterator.type, iterator.isGlobal);

  std::cout << "--------------------" << std::endl
            << "- Relocation Table -" << std::endl;

  count = 0;
  for (auto iterator : returnValue.relocationTable)

    printf("[%08x]: Name = %s; Address = %08x; OpCode = %x\n", (count++) * 4,
           iterator.symbolName.c_str(), iterator.address, iterator.opCode);
}

void Assembler::outputToFile(std::ostream &outFile, FileObject &fileObject) {
  // * Header
  ObjectHeader header = {
      static_cast<uint32_t>(fileObject.textSegment.size()),
      static_cast<uint32_t>(fileObject.dataSegment.size()),
      static_cast<uint32_t>(fileObject.symbolTable.size()),
      static_cast<uint32_t>(fileObject.relocationTable.size())};

  outFile.write((char *)&header, sizeof(header));

  // * text segment
  for (auto iterator : fileObject.textSegment)
    outFile.write((char *)&iterator, sizeof(iterator));

  // * data segment
  for (auto iterator : fileObject.dataSegment)
    outFile.write((char *)&iterator, sizeof(iterator));

  // * symbol table
  for (auto iterator : fileObject.symbolTable) {
    // Write name (size() + 1 includes null terminator)
    outFile.write((char *)iterator.name.c_str(), iterator.name.size() + 1);

    // Write address
    outFile.write((char *)&iterator.address, sizeof(iterator.address));

    // Write type
    outFile.write((char *)&iterator.type, sizeof(iterator.type));

    // Write isGlobal
    outFile.write((char *)&iterator.isGlobal, sizeof(iterator.isGlobal));
  }

  // * relocation table
  for (auto iterator : fileObject.relocationTable) {
    // Write name (size() + 1 includes null terminator)
    outFile.write((char *)iterator.symbolName.c_str(),
                  iterator.symbolName.size() + 1);

    // Write address
    outFile.write((char *)&iterator.address, sizeof(iterator.address));

    // Write opCode
    outFile.write((char *)&iterator.opCode, sizeof(iterator.opCode));
  }
}

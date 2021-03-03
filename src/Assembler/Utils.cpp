#include "Utils.hpp"
#include "../Common/Symbol.hpp"
#include <assert.h>
#include <iostream>
#include <map>

std::vector<std::string> getOperands(std::stringstream &ss) {

  std::vector<std::string> returnValue;
  std::string buffer;
  while (std::getline(ss, buffer, ',')) {

    u_int32_t spaceAtBeginning = 0;
    u_int32_t spaceAtEnd = 0;

    // Remove spaces at the beginning
    for (auto iterator : buffer) {
      if (IS_SPACE_OR_TAB(iterator))
        spaceAtBeginning++;
      else
        break;
    }

    // Remove spaces at the end
    for (int i = buffer.size() - 1; i >= 0; i--) {
      if (IS_SPACE_OR_TAB(buffer[i]))
        spaceAtEnd++;
      else
        break;
    }

    returnValue.push_back(buffer.substr(
        spaceAtBeginning, buffer.size() - spaceAtBeginning - spaceAtEnd));
  }

  return returnValue;
}

InstructionsAvailable resolveInstruction(std::string &instruction) {
  static const std::map<std::string, InstructionsAvailable> instructionMap{
      {"add", ADD},     {"addu", ADDU}, {"sub", SUB},
      {"subu", SUBU},   {"mult", MULT}, {"multu", MULTU},
      {"div", DIV},     {"divu", DIVU}, {"slt", SLT},
      {"sltu", SLTU},   {"and", AND},   {"or", OR},
      {"nor", NOR},     {"xor", XOR},   {"addi", ADDI},
      {"addiu", ADDIU}, {"slti", SLTI}, {"sltiu", SLTIU},
      {"andi", ANDI},   {"ori", ORI},   {"xori", XORI},
      {"lw", LW},       {"sw", SW},     {"lbu", LBU},
      {"lb", LB},       {"sb", SB},     {"lui", LUI},
      {"beq", BEQ},     {"bne", BNE},   {"blez", BLEZ},
      {"bgtz", BGTZ},   {"bltz", BLTZ}, {"j", J},
      {"jal", JAL},     {"jr", JR},     {"jalr", JALR},
      {"mfhi", MFHI},   {"mflo", MFLO}, {"syscall", SYSCALL_INST},
      {"srl", SRL},     {"sll", SLL}};

  auto result = instructionMap.find(instruction);
  if (result == instructionMap.end())
    throw std::runtime_error("Instruction name not found: " + instruction);

  return result->second;
}

uint8_t getOpCode(InstructionsAvailable &instruction) {
  switch (instruction) {
  case ADD:
  case ADDU:
  case SUB:
  case SUBU:
  case MULT:
  case MULTU:
  case DIV:
  case DIVU:
  case SLT:
  case SLTU:
  case AND:
  case OR:
  case NOR:
  case XOR:
  case JR:
  case JALR:
  case MFHI:
  case MFLO:
  case SYSCALL_INST:
  case SRL:
  case SLL:
    return 0;
    break;
  case ADDI:
    return 8;
    break;
  case ADDIU:
    return 9;
    break;
  case SLTI:
    return 10;
    break;
  case SLTIU:
    return 11;
    break;
  case ANDI:
    return 12;
    break;
  case ORI:
    return 13;
    break;
  case XORI:
    return 14;
    break;
  case LW:
    return 35;
    break;
  case SW:
    return 43;
    break;
  case LBU:
    return 36;
    break;
  case LB:
    return 32;
    break;
  case SB:
    return 40;
    break;
  case LUI:
    return 15;
    break;
  case BEQ:
    return 4;
    break;
  case BNE:
    return 5;
    break;
  case BLEZ:
    return 6;
    break;
  case BGTZ:
    return 7;
    break;
  case BLTZ:
    return 1;
    break;
  case J:
    return 2;
    break;
  case JAL:
    return 3;
    break;
  }

  throw std::runtime_error("[getOpCode]: code not found: " +
                           std::to_string(instruction));
}

uint8_t getRegisterNumberFromString(std::string &registerString) {
  if (registerString[0] != '$')
    throw std::runtime_error("Invalid register operand: " + registerString);

  static const std::map<std::string, uint8_t> registersNumberMap{
      {"0", 0},   {"1", 1},   {"2", 2},    {"3", 3},   {"4", 4},   {"5", 5},
      {"6", 6},   {"7", 7},   {"8", 8},    {"9", 9},   {"10", 10}, {"11", 11},
      {"12", 12}, {"13", 13}, {"14", 14},  {"15", 15}, {"16", 16}, {"17", 17},
      {"18", 18}, {"19", 19}, {"20", 20},  {"21", 21}, {"22", 22}, {"23", 23},
      {"24", 24}, {"25", 25}, {"26", 26},  {"27", 27}, {"28", 28}, {"29", 29},
      {"30", 30}, {"31", 31}, {"zero", 0}, {"at", 1},  {"v0", 2},  {"v1", 3},
      {"a0", 4},  {"a1", 5},  {"a2", 6},   {"a3", 7},  {"t0", 8},  {"t1", 9},
      {"t2", 10}, {"t3", 11}, {"t4", 12},  {"t5", 13}, {"t6", 14}, {"t7", 15},
      {"s0", 16}, {"s1", 17}, {"s2", 18},  {"s3", 19}, {"s4", 20}, {"s5", 21},
      {"s6", 22}, {"s7", 23}, {"t8", 24},  {"t9", 25}, {"k0", 26}, {"k1", 27},
      {"gp", 28}, {"sp", 29}, {"fp", 30},  {"ra", 31}};

  registerString.erase(registerString.begin());

  auto result = registersNumberMap.find(registerString);
  if (result == registersNumberMap.end())
    throw std::runtime_error("Invalid register operand: " + registerString);

  return result->second;
}

std::string removeBorderSpaces(std::string &input) {
  int beginningSpaceIndex = 0;
  int endSpacesCount = 0;

  int stringSize = input.size();

  // Count spaces at the beginning
  for (int i = 0; i < stringSize; i++) {
    if (IS_SPACE_OR_TAB(input[i]))
      beginningSpaceIndex++;
    else
      break;
  }

  // Count spaces at the end
  for (int i = stringSize - 1; i > beginningSpaceIndex; i++) {
    if (IS_SPACE_OR_TAB(input[i]))
      endSpacesCount++;
    else
      break;
  }

  return input.substr(beginningSpaceIndex,
                      stringSize - beginningSpaceIndex - endSpacesCount);
}

std::pair<std::string, std::string> *getOffsetAndRegister(std::string &input) {
  std::stringstream ss(input);
  std::string immediateString;
  std::string registerString;

  if (!std::getline(ss, immediateString, '('))
    throw std::runtime_error(
        "Error during parsing of immediate, argument invalid: " + input);

  if (!std::getline(ss, registerString, ')')) {
    std::cerr << "[WARNING]: Implicit ($gp) register: " << input << std::endl;
    registerString = "$gp";
  } else {
    registerString = removeBorderSpaces(registerString);
  }

  immediateString = removeBorderSpaces(immediateString);

  // When not specified, set it to zero by default
  if (immediateString.size() == 0) {
    std::cerr << "[WARNING]: Implicit 0([...]) offset" << std::endl;
    immediateString = "0";
  }

  return new std::pair<std::string, std::string>(immediateString,
                                                 registerString);
}

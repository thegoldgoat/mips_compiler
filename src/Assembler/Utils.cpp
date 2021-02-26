#include "Utils.h"
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
      if (iterator == ' ')
        spaceAtBeginning++;
      else
        break;
    }

    // Remove spaces at the end
    for (int i = buffer.size() - 1; i >= 0; i--) {
      if (buffer[i] == ' ')
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
      {"add", ADD},   {"addu", ADDU},   {"sub", SUB},   {"subu", SUBU},
      {"mult", MULT}, {"multu", MULTU}, {"div", DIV},   {"divu", DIVU},
      {"slt", SLT},   {"sltu", SLTU},   {"and", AND},   {"or", OR},
      {"nor", NOR},   {"xor", XOR},     {"addi", ADDI}, {"addiu", ADDIU},
      {"slti", SLTI}, {"sltiu", SLTIU}, {"andi", ANDI}, {"ori", ORI},
      {"xori", XORI}, {"lw", LW},       {"sw", SW},     {"lbu", LBU},
      {"lb", LB},     {"sb", SB},       {"lui", LUI},   {"beq", BEQ},
      {"bne", BNE},   {"blez", BLEZ},   {"bgtz", BGTZ}, {"bltz", BLTZ},
      {"j", J},       {"jal", JAL},     {"jr", JR},     {"jalr", JALR},
      {"nop", NOP},   {"mfhi", MFHI},   {"mflo", MFLO},
  };

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
  case NOP:
  case MFHI:
  case MFLO:
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
    return 12;
    break;
  case ANDI:
    return 13;
    break;
  case ORI:
    return 14;
    break;
  case XORI:
    return 15;
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

#define SET_FUNCT(target, code) target |= code

void populateInstructionWithOperands(std::vector<std::string> &operands,
                                     uint32_t *instruction,
                                     InstructionsAvailable instructionEnum) {

  try {

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
      std::cout << "[DEBUG]: "
                << "Arithmetic opperands: rd=" << operands.at(0)
                << "; rs=" << operands.at(1) << "; rt=" << operands.at(2)
                << std::endl;

      *instruction |= getRegisterNumberFromString(operands.at(1)) << 21;
      *instruction |= getRegisterNumberFromString(operands.at(2)) << 16;
      *instruction |= getRegisterNumberFromString(operands.at(0)) << 11;

      break;
    case MULT:
    case MULTU:
    case DIV:
    case DIVU:
      // Syntax: Rs, Rt with order in instruction as Rs, Rt
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
      break;

    case LW:
    case SW:
    case LBU:
    case LB:
    case SB:
      // Syntax: Rt, offset(Rs) with order in memory as Rs, Rt, offset
      break;
    case LUI:
      // Syntax: Rt, Immediate with order in memory as 00000, rt, Immediate
      break;
    case BLEZ:
    case BGTZ:
    case BLTZ:
      // Syntax: Rs, offset with order in memory as rs, 00000, offset
      break;
    case J:
    case JAL:
      // Syntax: Immediate with order in memory as Immediate
      break;
    case JALR:
      // Syntax: Rd, Rs with order in memory as rs, 00000, Rd, 00000, 001001
      break;

    case JR:
      // Syntax: Rs with order in memory as rs, 00000, 00000, 00000, 001000
      break;
    case NOP:
      // All zeros
      *instruction = 0;
      break;
    case MFHI:
    case MFLO:
      // Syntax: Rd with order in memory as 00000, 00000, rd,  00000,
      // funct_code
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
      SET_FUNCT(*instruction, 0x0);
      break;
    case SLTU:
      SET_FUNCT(*instruction, 0x0);
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
    default:
      break;
    }
  } catch (std::out_of_range &exception) {
    throw std::runtime_error("Missing operand");
  }
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

#define MAX_INT_16 32767
#define MIN_INT_16 -32768

int16_t getImmediateFromString(std::string &string) {
  try {
    auto result = std::stoi(string);

    if (result > MAX_INT_16 || result < MIN_INT_16) {
      throw std::out_of_range("");
    }

    return result;
  } catch (std::invalid_argument &) {
    throw std::runtime_error("Immediate invalid: " + string);
  } catch (std::out_of_range &) {
    throw std::runtime_error("Immediate out of range: " + string);
  }
}
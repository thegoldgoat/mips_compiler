#include "Utils.h"
#include <map>

std::vector<std::string> getOperands(std::stringstream &ss) {

  std::vector<std::string> returnValue;
  std::string buffer;
  while (std::getline(ss, buffer, ','))
    returnValue.push_back(buffer);

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
}

void populateInstructionWithOperands(std::vector<std::string> &operands,
                                     uint32_t *instruction,
                                     InstructionsAvailable instructionEnum) {

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
    // Syntax: Rd, Rs, Rt with order in instruction as Rs, Rt, Rd, funct_code
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
    // Syntax: Rt, Rs, Immediate with order in instruction as Rs, Rt, Immediate
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
    // Syntax: Rd with order in memory as 00000, 00000, rd,  00000, funct_code
    break;
  }
}

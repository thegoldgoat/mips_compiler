#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <stdint.h>

enum InstructionsAvailable {
  ADD,
  ADDU,
  SUB,
  SUBU,
  MULT,
  MULTU,
  DIV,
  DIVU,
  SLT,
  SLTU,
  AND,
  OR,
  NOR,
  XOR,
  ADDI,
  ADDIU,
  SLTI,
  SLTIU,
  ANDI,
  ORI,
  XORI,
  LW,
  SW,
  LBU,
  LB,
  SB,
  LUI,
  BEQ,
  BNE,
  BLEZ,
  BGTZ,
  BLTZ,
  J,
  JAL,
  JR,
  JALR,
  NOP,
  MFHI,
  MFLO
};

std::vector<std::string> getOperands(std::stringstream &ss);

InstructionsAvailable resolveInstruction(std::string &instruction);

uint8_t getOpCode(InstructionsAvailable &instruction);

void populateInstructionWithOperands(std::vector<std::string> &operands, uint32_t *instruction, InstructionsAvailable instructionEnum);


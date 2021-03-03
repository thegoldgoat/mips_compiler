#pragma once
#include <string>

enum PseudoInstructions {
  MOVE,
  CLEAR,
  LOAD_IMMEDIATE,
  LOAD_ADDRESS,
  BRANCH_GREATER_THAN,
  BRANCH_LESS_THAN,
  BRANCH_GREATER_EQUAL,
  BRANCH_LESS_EQUAL,
  NOT,
  NOP
};

PseudoInstructions resolvePseudoInstruction(std::string instructionString);
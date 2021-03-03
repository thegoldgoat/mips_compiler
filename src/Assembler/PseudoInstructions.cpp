#include "PseudoInstructions.hpp"

#include <map>
#include <stdexcept>

PseudoInstructions resolvePseudoInstruction(std::string instructionString) {
  std::map<std::string, PseudoInstructions> pseudoInstructionsMap = {
      {"move", MOVE},
      {"clear", CLEAR},
      {"li", LOAD_IMMEDIATE},
      {"la", LOAD_ADDRESS},
      {"bgt", BRANCH_GREATER_THAN},
      {"blt", BRANCH_LESS_THAN},
      {"bge", BRANCH_GREATER_EQUAL},
      {"ble", BRANCH_LESS_EQUAL},
      {"not", NOT},
      {"nop", NOP}};

  auto result = pseudoInstructionsMap.find(instructionString);

  if (result == pseudoInstructionsMap.end())
    throw std::runtime_error("Instruction not found: " + instructionString);

  return result->second;
}

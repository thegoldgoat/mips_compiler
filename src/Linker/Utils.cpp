#include "Utils.hpp"

#include <map>
#include <stdexcept>
#include <stdint.h>

InstructionsAvailable getInstructionFromOpCode(uint8_t opCode) {
  // This map cannot understand the difference between a "SUB" and an "ADD" but
  // it doesn't matter since that type of instructions don't need relocation.
  static std::map<uint8_t, InstructionsAvailable> instructionMap = {
      {2, J},     {3, JAL},    {15, LUI},  {4, BEQ},  {5, BNE},
      {6, BLEZ},  {7, BGTZ},   {1, BLTZ},  {35, LW},  {43, SW},
      {36, LBU},  {32, LB},    {40, SB},   {8, ADDI}, {9, ADDIU},
      {10, SLTI}, {11, SLTIU}, {12, ANDI}, {13, ORI}, {14, XORI}};

  auto result = instructionMap.find(opCode);

  if (result == instructionMap.end())
    throw std::runtime_error(std::string("Couln't resolve opCode: ") +
                             std::to_string(opCode));

  return result->second;
}
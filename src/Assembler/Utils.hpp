#pragma once

#define IS_SPACE_OR_TAB(targetChar) targetChar == ' ' || targetChar == '\t'

#include <sstream>
#include <string>
#include <vector>

#include <stdint.h>

#include "../Common/InstructionAvailable.hpp"

std::vector<std::string> getOperands(std::stringstream &ss);

InstructionsAvailable resolveInstruction(std::string &instruction);

uint8_t getOpCode(InstructionsAvailable &instruction);

uint8_t getRegisterNumberFromString(std::string &registerString);

std::string removeBorderSpaces(std::string &input);

std::pair<std::string, std::string> *getOffsetAndRegister(std::string &input);

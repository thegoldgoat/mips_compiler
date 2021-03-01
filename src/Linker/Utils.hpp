#pragma once

#include "../Common/InstructionAvailable.hpp"
#include <stdint.h>

InstructionsAvailable getInstructionFromOpCode(uint8_t opCode);
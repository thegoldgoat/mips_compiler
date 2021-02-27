#pragma once
#include "../Symbol/Symbol.h"
#include <stdint.h>

struct ObjectHeader {
  uint32_t textSegmentSize;
  uint32_t dataSegmentSize;
  uint32_t symbolTableSize;
  uint32_t relocationTableSize;
};
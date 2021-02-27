#pragma once
#include "Relocation.hpp"
#include "Symbol.hpp"
#include <stdint.h>
#include <vector>

struct ObjectHeader {
  uint32_t textSegmentSize;
  uint32_t dataSegmentSize;
  uint32_t symbolTableSize;
  uint32_t relocationTableSize;
};

struct FileObject {
  std::vector<Relocation> relocationTable;
  std::vector<Symbol> symbolTable;
  std::vector<uint32_t> textSegment;
  std::vector<uint32_t> dataSegment;
};
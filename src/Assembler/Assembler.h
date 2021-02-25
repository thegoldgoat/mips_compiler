#pragma once

#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>

#include "../Relocation/Relocation.h"
#include "../Symbol/Symbol.h"

struct FileObject {
  std::vector<Relocation> relocationTable;
  std::vector<Symbol> symbolTable;
  std::vector<uint32_t> textSegment;
  std::vector<uint32_t> dataSegment;
};

FileObject assembleFile(std::ifstream &fileContent);
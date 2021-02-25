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

enum Section { TEXT, DATA, NONE };

class Assembler {
public:
  Assembler(std::ifstream *fileContent);

  FileObject assemble();

private:
  FileObject returnValue;
  Section currentSection = NONE;
  std::ifstream *fileStream;

  void makeSymbolGlobal(std::string &symbolName);

  std::string removeBeginningSpaces(std::string input);

  void parseLine(std::string line);
};
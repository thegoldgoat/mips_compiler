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

  std::string stringBuffer;

  void makeSymbolGlobal(std::string &symbolName);
  void addNewSymbol(std::string name, SymbolType type);

  std::string removeBeginningSpacesAndComment(std::string &input);

  void parseLine(std::string &line);

  // Helper functions for parseLine
  void dotLine(std::string &line);
  void instructionLine(std::string &line);

  void parseWord(std::stringstream &ss);
  void parseInstruction(std::string instructionCode, std::stringstream &ss);

  void parseSpace(std::stringstream &ss);
};

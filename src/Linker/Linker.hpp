#pragma once
#include "../Common/ObjectStructs.hpp"
#include <fstream>
#include <map>
#include <vector>

#define GLOBAL_POINTER 0x10008000

struct ParsedObject {
  std::map<std::string, SymbolForMap> localSymbols;
  std::vector<Relocation> relocationTable;
};

class Linker {
public:
  Linker();

  void link(std::vector<std::string> &inputFiles);

  void prettyPrintExecutable();

private:
  ParsedObject parseObjectFile(std::string &fileName);

  void doRelocation(Relocation &relocation,
                    std::map<std::string, SymbolForMap> &localSymbols);

  std::vector<ParsedObject> fileObjects;

  std::map<std::string, SymbolForMap> globalSymbols;

  std::vector<uint32_t> textSegment;
  std::vector<uint32_t> dataSegment;

  uint32_t textSegmentBase;
  uint32_t dataSegmentBase;
};
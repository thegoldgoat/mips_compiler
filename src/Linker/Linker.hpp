#pragma once
#include "../Common/ObjectStructs.hpp"
#include <fstream>
#include <map>
#include <vector>

struct ParsedObject {
  std::map<std::string, SymbolForMap> localSymbols;
  std::vector<Relocation> relocationTable;
};

class Linker {
public:
  Linker();

  void link(std::vector<std::string> &inputFiles);

private:
  ParsedObject parseObjectFile(std::string &fileName);

  std::vector<ParsedObject> fileObjects;

  std::map<std::string, SymbolForMap> globalSymbols;

  std::vector<uint32_t> textSegment;
  std::vector<uint32_t> dataSegment;

  uint32_t textSegmentBase;
  uint32_t dataSegmentBase;
};
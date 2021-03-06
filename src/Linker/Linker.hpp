#pragma once
#include "../Common/ObjectStructs.hpp"
#include <fstream>
#include <map>
#include <vector>

#define GLOBAL_POINTER 0x10008000
#define TEXT_SEGMENT_BEGIN 0x00400000
#define DATA_SEGMENT_BEGIN 0x10000000

struct ParsedObject {
  std::map<std::string, SymbolForMap> localSymbols;
  std::vector<Relocation> relocationTable;
};

struct ExeHeader {
  uint32_t textSegmentSize;
  uint32_t dataSegmentSize;
  uint32_t entryPointOffset;
};

class Linker {
public:
  Linker();

  void link(std::vector<std::string> &inputFiles);

  void outputToFile(std::string path);

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
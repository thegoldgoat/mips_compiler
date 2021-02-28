#pragma once
#include "../Common/ObjectStructs.hpp"
#include <fstream>
#include <map>
#include <vector>

class Linker {
public:
  Linker();

  void link(std::vector<std::istream *> &inputFiles);

private:
  FileObject parseObjectFile(std::istream &inputFile);

  std::vector<FileObject> fileObjects;

  std::map<std::string, SymbolForMap> globalSymbols;
  std::map<std::string, SymbolForMap> localSymbols;
};
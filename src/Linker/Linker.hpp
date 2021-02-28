#pragma once
#include "../Common/ObjectStructs.hpp"
#include <fstream>
#include <vector>

class Linker {
public:
  Linker();

  void link(std::vector<std::istream *> &inputFiles);

private:
  FileObject parseObjectFile(std::istream &inputFile);

  std::vector<FileObject> fileObjects;
};
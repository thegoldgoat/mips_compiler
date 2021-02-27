#pragma once
#include "../Common/ObjectStructs.hpp"
#include <fstream>
#include <vector>

class Linker {
  Linker(std::vector<std::istream> &inputFiles);

private:
  FileObject parseObjectFile(std::istream inputFile);
};
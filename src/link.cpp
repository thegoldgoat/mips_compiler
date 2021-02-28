#include "Linker/Linker.hpp"
#include <fstream>
#include <iostream>

int main() {

  std::ifstream inputFile("/tmp/canex");

  if (!inputFile.is_open()) {
    perror("Couldn't open input object file");
    return -1;
  }

  Linker linker;
  std::vector<std::istream *> inputFiles;

  inputFiles.push_back(&inputFile);

  linker.link(inputFiles);
}

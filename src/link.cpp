#include "Linker/Linker.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>

#define PRINT_USAGE                                                            \
  std::cerr << "Usage: " << argv[0]                                            \
            << " -o <output_path> <input_file1> <input_file2> [...]"           \
            << std::endl

int main(int argc, char **argv) {

  if (argc < 4) {
    PRINT_USAGE;
    return -1;
  }

  if (strcmp(argv[1], "-o")) {
    PRINT_USAGE;
    return -1;
  }

  std::vector<std::string> inputFiles;

  for (int i = 3; i < argc; i++) {
    inputFiles.push_back(argv[i]);
  }

  Linker linker;
  linker.link(inputFiles);

  linker.outputToFile(argv[2]);
}

#include "Assembler/Assembler.hpp"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>

int main(int argc, char **argv) {

  std::cout << "argc = " << argc << std::endl;
  if (argc < 3 || argc > 4) {
    std::cerr << "Usage: " << argv[0]
              << " <input_path> <output_path> [--verbose]" << std::endl;
    return -1;
  }

  std::ifstream inputFile(argv[1]);
  if (!inputFile.is_open()) {
    perror("Error while opening input file");
    return -1;
  }

  std::ofstream outFile(argv[2], std::ios::binary);

  if (!outFile.is_open()) {
    perror("Error while opening output file");
    return -1;
  }

  Assembler assembler(&inputFile);

  assembler.assemble();

  inputFile.close();

  if (argc == 4 && strcmp(argv[3], "--verbose") == 0) {
    assembler.prettyPrintObject();
  }

  FileObject outObj = assembler.getObject();

  Assembler::outputToFile(outFile, outObj);

  outFile.close();

  return 0;
}

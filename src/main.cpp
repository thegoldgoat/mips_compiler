#include "Assembler/Assembler.h"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>

int main() {
  std::ifstream inputFile("/home/andrea/Documenti/test.mips");
  if (!inputFile.is_open()) {
    perror("Error while opening input file");
    return -1;
  }

  Assembler assembler(&inputFile);

  assembler.assemble();

  inputFile.close();

  assembler.prettyPrintObject();
  std::ofstream outFile("/tmp/canex", std::ios::binary);

  FileObject outObj = assembler.getObject();

  if (!outFile.is_open()) {
    perror("Error while opening output file");
    return -1;
  }

  Assembler::outputToFile(outFile, outObj);

  outFile.close();

  return 0;
}

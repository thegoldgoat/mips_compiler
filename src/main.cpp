#include "Assembler/Assembler.h"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>

int main() {
  std::ifstream file1("/home/andrea/Documenti/test.mips");
  if (!file1.is_open()) {
    perror("Error while opening file");
    return -1;
  }

  Assembler assembler(&file1);

  assembler.assemble();
}

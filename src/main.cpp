#include "Assembler/Assembler.h"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>

int main() {
  std::ifstream file1("/home/andrea/Documenti/test.mips");
  if (file1.is_open())
    assembleFile(file1);
  else
    std::cerr << "Error while opening file: " << strerror(errno) << std::endl;
}

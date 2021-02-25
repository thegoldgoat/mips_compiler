#include "Assembler.h"
#include <iostream>
#include <sstream>

#include "./Utils.h"

enum Section { TEXT, DATA, NONE };

FileObject assembleFile(std::ifstream &fileContent) {

  std::string stringBuffer;

  FileObject returnValue;

  Section currentSection = NONE;

  while (std::getline(fileContent, stringBuffer)) {

    // * Remove trailing spaces
    uint32_t endSpaceIndex = 0;

    for (auto iterator : stringBuffer) {
      if (iterator != ' ')
        break;
      endSpaceIndex++;
    }

    std::string noSpaceString = stringBuffer.substr(endSpaceIndex);

    std::cout << "Parsing " << noSpaceString << std::endl;

    if (noSpaceString[0] == ';') {
      continue;
    } else if (noSpaceString[0] == '.') {
      // Section or other special keywords
      std::stringstream noSpaceBuffer(noSpaceString);
      noSpaceBuffer >> stringBuffer;

      std::cout << "Special word: " << stringBuffer << std::endl;

      if (stringBuffer == ".text") {
        currentSection = TEXT;

      } else if (stringBuffer == ".data") {
        currentSection = DATA;
      } else if (stringBuffer == ".globl") {
        // Read symbol name
        noSpaceBuffer >> stringBuffer;

        std::cout << "[.globl]: exporting symbol named " << stringBuffer
                  << std::endl;

        makeSymbolGlobal(returnValue.symbolTable, stringBuffer);
      }

    } else {
      // TODO:
    }
  }

  return returnValue;
}

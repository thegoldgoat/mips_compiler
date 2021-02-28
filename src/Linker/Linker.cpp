#include "Linker.hpp"

#include <stdio.h>

Linker::Linker() {}

void Linker::link(std::vector<std::istream *> &inputFiles) {
  for (auto iterator : inputFiles) {
    fileObjects.push_back(parseObjectFile(*iterator));
  }
}

FileObject Linker::parseObjectFile(std::istream &inputFile) {
  ObjectHeader header;
  FileObject returnValue;

  uint32_t uint32Buffer;

  // Read Header
  inputFile.read((char *)&header, sizeof(ObjectHeader));

  // Read Text Segment
  for (int i = 0; i < header.textSegmentSize; i++) {
    inputFile.read((char *)&uint32Buffer, sizeof(uint32_t));
    returnValue.textSegment.push_back(uint32Buffer);
  }

  // Read Data Segment
  for (int i = 0; i < header.dataSegmentSize; i++) {
    inputFile.read((char *)&uint32Buffer, sizeof(uint32_t));
    returnValue.dataSegment.push_back(uint32Buffer);
  }

  // Read Symbol Table

  Symbol tempSymbol;
  for (int i = 0; i < header.symbolTableSize; i++) {
    std::getline(inputFile, tempSymbol.name, '\0');
    inputFile.read((char *)&tempSymbol.address, sizeof(tempSymbol.address));
    inputFile.read((char *)&tempSymbol.type, sizeof(tempSymbol.type));
    inputFile.read((char *)&tempSymbol.isGlobal, sizeof(tempSymbol.isGlobal));
    returnValue.symbolTable.push_back(tempSymbol);
  }

  // Read Relocation Table
  Relocation tempRelocation;
  for (int i = 0; i < header.symbolTableSize; i++) {
    std::getline(inputFile, tempRelocation.symbolName, '\0');
    inputFile.read((char *)&tempRelocation.address,
                   sizeof(tempRelocation.address));
    inputFile.read((char *)&tempRelocation.opCode,
                   sizeof(tempRelocation.opCode));
    returnValue.relocationTable.push_back(tempRelocation);
  }

  return returnValue;
}

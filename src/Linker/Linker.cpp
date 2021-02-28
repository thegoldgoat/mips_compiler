#include "Linker.hpp"

#include <stdio.h>

Linker::Linker() {}

void Linker::link(std::vector<std::string> &inputFiles) {
  for (auto iterator : inputFiles) {
    fileObjects.push_back(parseObjectFile(iterator));
  }

  return;
}

ParsedObject Linker::parseObjectFile(std::string &fileName) {

  std::ifstream inputFile(fileName);
  if (!inputFile.is_open()) {
    perror("Couldn't open file");
    throw std::runtime_error("Couldn't open file");
  }

  std::map<std::string, SymbolForMap> localSymbols;
  std::vector<Relocation> relocationTable;

  ObjectHeader header;
  uint32_t uint32Buffer;

  // Read Header
  inputFile.read((char *)&header, sizeof(ObjectHeader));

  printf("[DEBUG]: textSize = %08x; dataSize = %08x; symbolTableSize = %08x; "
         "relocationTableSize = %08x\n",
         header.textSegmentSize, header.dataSegmentSize, header.symbolTableSize,
         header.relocationTableSize);

  // Read Text Segment
  for (int i = 0; i < header.textSegmentSize; i++) {
    inputFile.read((char *)&uint32Buffer, sizeof(uint32_t));
    textSegment.push_back(uint32Buffer);
  }

  // Read Data Segment
  for (int i = 0; i < header.dataSegmentSize; i++) {
    inputFile.read((char *)&uint32Buffer, sizeof(uint32_t));
    dataSegment.push_back(uint32Buffer);
  }

  // Read Symbol Table
  Symbol tempSymbol;
  for (int i = 0; i < header.symbolTableSize; i++) {
    std::getline(inputFile, tempSymbol.name, '\0');
    inputFile.read((char *)&tempSymbol.address, sizeof(tempSymbol.address));
    inputFile.read((char *)&tempSymbol.type, sizeof(tempSymbol.type));
    inputFile.read((char *)&tempSymbol.isGlobal, sizeof(tempSymbol.isGlobal));

    if (tempSymbol.isGlobal)
      globalSymbols[tempSymbol.name] = {tempSymbol.type, tempSymbol.address};
    else
      localSymbols[tempSymbol.name] = {tempSymbol.type, tempSymbol.address};
  }

  // Read Relocation Table
  Relocation tempRelocation;
  for (int i = 0; i < header.symbolTableSize; i++) {
    std::getline(inputFile, tempRelocation.symbolName, '\0');
    inputFile.read((char *)&tempRelocation.address,
                   sizeof(tempRelocation.address));
    inputFile.read((char *)&tempRelocation.opCode,
                   sizeof(tempRelocation.opCode));
    relocationTable.push_back(tempRelocation);
  }

  return {localSymbols, relocationTable};
}

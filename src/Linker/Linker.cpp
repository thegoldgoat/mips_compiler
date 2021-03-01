#include "Linker.hpp"

#include <assert.h>
#include <stdio.h>
#include <string>

#include "Utils.hpp"

Linker::Linker() {}

void Linker::link(std::vector<std::string> &inputFiles) {
  for (auto iterator : inputFiles) {
    // Update segments base (needed for relocation and symbol table)
    dataSegmentBase = dataSegment.size() * 4;
    textSegmentBase = textSegment.size() * 4;

    fileObjects.push_back(parseObjectFile(iterator));
  }

  // Check that there is a global text symbol named 'MAIN':
  auto result = globalSymbols.find("MAIN");

  if (result == globalSymbols.end())
    throw std::runtime_error("Missing MAIN symbol for entry point");

  if (result->second.type != SYMBOL_TEXT)
    throw std::runtime_error("MAIN is not a TEXT symbol");

  for (auto &iterator : fileObjects) {
    for (auto &currentRelocation : iterator.relocationTable) {
      doRelocation(currentRelocation, iterator.localSymbols);
    }
  }

  return;
}

void Linker::doRelocation(Relocation &relocation,
                          std::map<std::string, SymbolForMap> &localSymbols) {
  // Do the relocation based on the type of instruction

  SymbolForMap symbolForMap;

  auto result = localSymbols.find(relocation.symbolName);

  if (result == localSymbols.end()) {
    result = globalSymbols.find(relocation.symbolName);

    if (result == globalSymbols.end()) {
      throw std::runtime_error("Symbol not found: " + relocation.symbolName);
    }
  }

  symbolForMap = result->second;

  switch (getInstructionFromOpCode(relocation.opCode)) {
  case LW:
  case SW:
  case LBU:
  case LB:
  case SB:
    // Write in the offset address - $gp
    textSegment.at(relocation.address / 4) |=
        symbolForMap.address - GLOBAL_POINTER;
    break;
  case LUI:
    textSegment.at(relocation.address / 4) |= symbolForMap.address;
    break;
  case BEQ:
  case BNE:
  case BLEZ:
  case BGTZ:
  case BLTZ:
    textSegment.at(relocation.address / 4) |=
        ((symbolForMap.address - relocation.address + 4) / 4) & 0x0000ffff;
    break;
  case J:
  case JAL:
    textSegment.at(relocation.address / 4) |= symbolForMap.address / 4;
    break;
  default:
    throw std::runtime_error("Trying to relocate an unsupported instruction: " +
                             std::to_string(relocation.opCode));
    break;
  }
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
  SymbolForMap tempSymbolForMap;
  for (int i = 0; i < header.symbolTableSize; i++) {
    std::getline(inputFile, tempSymbol.name, '\0');
    inputFile.read((char *)&tempSymbol.address, sizeof(tempSymbol.address));
    inputFile.read((char *)&tempSymbol.type, sizeof(tempSymbol.type));
    inputFile.read((char *)&tempSymbol.isGlobal, sizeof(tempSymbol.isGlobal));

    assert(tempSymbol.type != SYMBOL_NONE);

    tempSymbolForMap.type = tempSymbol.type;
    if (tempSymbol.type == SYMBOL_DATA)
      tempSymbolForMap.address = tempSymbol.address + dataSegmentBase;
    else
      tempSymbolForMap.address = tempSymbol.address + textSegmentBase;

    if (tempSymbol.isGlobal)
      globalSymbols[tempSymbol.name] = tempSymbolForMap;
    else
      localSymbols[tempSymbol.name] = tempSymbolForMap;
  }

  // Read Relocation Table
  Relocation tempRelocation;
  for (int i = 0; i < header.symbolTableSize; i++) {
    std::getline(inputFile, tempRelocation.symbolName, '\0');
    inputFile.read((char *)&tempRelocation.address,
                   sizeof(tempRelocation.address));
    inputFile.read((char *)&tempRelocation.opCode,
                   sizeof(tempRelocation.opCode));

    tempRelocation.address += textSegmentBase;

    relocationTable.push_back(tempRelocation);
  }

  return {localSymbols, relocationTable};
}

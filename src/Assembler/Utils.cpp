#include "Utils.h"

std::vector<std::string> getOperands(std::stringstream &ss) {

  std::vector<std::string> returnValue;
  std::string buffer;
  while (std::getline(ss, buffer, ','))
    returnValue.push_back(buffer);

  return returnValue;
}

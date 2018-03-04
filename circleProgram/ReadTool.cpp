#include "ReadTool.h"
#include <fstream>
#include <stdexcept>

namespace ProjectName {

std::string ReadTool::readWholeFile(const std::string& path, size_t sizeGuess) {
  std::string result;

  result.reserve(sizeGuess);

  std::ifstream stream;
  stream.open(path);

  if( stream.bad() ) {
    std::string error = "Could not open file ";
    error += path;
    throw std::runtime_error(error);
  }

  char c;
  while( stream.good() ) {
    stream.get(c);
    result += c;
  }

  result.shrink_to_fit();

  return result;
}

}

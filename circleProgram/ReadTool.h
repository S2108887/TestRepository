#pragma once

#include <string>

namespace ProjectName {

class ReadTool {
 public:
  static std::string readWholeFile(const std::string& path, size_t sizeGuess=1000);
};

}

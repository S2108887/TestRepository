#pragma once

#include <memory>

class NumberGenerator {
 public:
  NumberGenerator();
  ~NumberGenerator();

  double operator()();
 
 private:
  class I;
  std::unique_ptr<I> imp;
};

#include "NumberGenerator.h"

#include <random>

class NumberGenerator::I {
 public:
  I() {
    std::random_device device;
    generator.seed( device() );
  }

  double operator()() {
    return distribution(generator);
  }

 private:
  std::mt19937_64 generator;

  std::uniform_real_distribution<> distribution{0.0,1.0};
  
};

NumberGenerator::NumberGenerator() : imp(new I()) { }
NumberGenerator::~NumberGenerator() = default;


double NumberGenerator::operator()() {
  return (*imp)();
}

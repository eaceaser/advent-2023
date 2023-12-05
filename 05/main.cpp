#include "lib.hpp"

#include <fstream>
#include <iostream>

int main() {
  std::ifstream input_file("input");

  if (input_file.is_open()) {
    std::cout << "part1: " << part1(input_file) << std::endl;
  }
}

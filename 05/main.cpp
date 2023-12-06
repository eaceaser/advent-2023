#include "lib.hpp"

#include <fstream>
#include <iostream>

int main() {
  std::ifstream input_file("input");

  if (input_file.is_open()) {
    std::cout << "part1: " << part1(input_file) << std::endl;
    input_file.clear();
    input_file.seekg(0, std::iostream::beg);
    std::cout << "part2: " << part2(input_file) << std::endl;
  }
}

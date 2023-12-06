#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "lib.hpp"
#include <doctest/doctest.h>

TEST_CASE("06-part1") {
  constexpr auto example = R"EOF(
Time:      7  15   30
Distance:  9  40  200
)EOF";
  std::stringstream ss(1 + example);
  CHECK(part1(ss) == 288);
}

TEST_CASE("06-part2") {
  constexpr auto example = R"EOF(
Time:      7  15   30
Distance:  9  40  200
)EOF";
  std::stringstream ss(1 + example);
  CHECK(part2(ss) == 71503);
}

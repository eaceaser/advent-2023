#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "lib.hpp"
#include <doctest/doctest.h>

TEST_CASE("07-part1") {
  constexpr auto input = R"FOO(
32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)FOO";
  std::stringstream ss(1 + input);
  CHECK(part1(ss) == 6440);
}

TEST_CASE("07-part2") {
  constexpr auto input = R"FOO(
32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)FOO";
  std::stringstream ss(1 + input);
  CHECK(part2(ss) == 5905);
}

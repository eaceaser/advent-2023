#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <lexy/action/parse.hpp>
#include <lexy/callback/container.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <vector>

namespace {
struct Card {
  int id;
  std::set<int> winning;
  std::vector<int> picks;
};

namespace grammar {
namespace dsl = lexy::dsl;

struct winning {
  static constexpr auto rule = dsl::list(dsl::integer<int>(dsl::digits<>));
  static constexpr auto value = lexy::as_collection<std::set<int>>;
};

struct picks {
  static constexpr auto rule = dsl::list(dsl::integer<int>(dsl::digits<>));
  static constexpr auto value = lexy::as_list<std::vector<int>>;
};

struct production {
  static constexpr auto rule =
      LEXY_LIT("Card") + dsl::integer<int>(dsl::digits<>) + dsl::colon +
      dsl::p<winning> + dsl::lit_c<'|'> + dsl::p<picks>;

  static constexpr auto whitespace = dsl::ascii::space;
  static constexpr auto value = lexy::construct<Card>;
};

} // namespace grammar
} // namespace

int part1(std::istream &input) {
  std::string line;
  int sum = 0;
  while (std::getline(input, line)) {
    auto str = lexy::string_input(line);
    auto result = lexy::parse<grammar::production>(str, lexy_ext::report_error);

    if (result.has_value()) {
      Card card = result.value();

      int count = std::ranges::count_if(
          card.picks, [&card](int i) { return card.winning.contains(i); });

      if (count > 0) {
        sum += pow(2, count - 1);
      }
    }
  }

  return sum;
}

int part2(std::istream &input) {
  std::string line;
  std::vector<Card> cards;
  while (std::getline(input, line)) {
    auto str = lexy::string_input(line);
    auto result = lexy::parse<grammar::production>(str, lexy_ext::report_error);

    if (result.has_value()) {
      Card card = result.value();
      cards.push_back(card);
    }
  }

  std::vector<int> counts(cards.size(), 1);
  for (int i = 0; auto card : cards) {
    int count = std::ranges::count_if(
        card.picks, [&card](int i) { return card.winning.contains(i); });

    for (int j = 1; j <= count; ++j) {
      counts[i + j] += counts[i];
    }

    ++i;
  }

  return std::reduce(counts.begin(), counts.end());
}
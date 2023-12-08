#include <algorithm>
#include <format>
#include <istream>
#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy/token.hpp>
#include <lexy_ext/report_error.hpp>
#include <map>
#include <ranges>
#include <vector>

enum class card_value {
  joker,
  two,
  three,
  four,
  five,
  six,
  seven,
  eight,
  nine,
  ten,
  jack,
  queen,
  king,
  ace,
};

enum class hand_value {
  high,
  pair,
  two_pair,
  three_of_a_kind,
  full_house,
  four_of_a_kind,
  five_of_a_kind,
};

class Hand {
public:
  Hand(card_value a, card_value b, card_value c, card_value d, card_value e);
  bool operator<(const Hand &other) const;

private:
  std::array<card_value, 5> hand;
  std::vector<std::pair<card_value, int>> sets;
  hand_value value;
};

Hand::Hand(card_value a, card_value b, card_value c, card_value d, card_value e)
    : hand{a, b, c, d, e} {
  using enum hand_value;
  using enum card_value;

  std::map<card_value, int> counts;
  for (auto card : this->hand) {
    counts.try_emplace(card, 0);
    counts[card]++;
  }

  int jokers = 0;
  if (const auto el = counts.find(joker); el != counts.end()) {
    jokers = el->second;
    counts.erase(el);
  }

  auto countsWithJokers = std::views::transform(counts, [jokers](auto el) {
    el.second += jokers;
    return el;
  });

  this->sets.insert(this->sets.end(), countsWithJokers.begin(),
                    countsWithJokers.end());
  std::ranges::sort(this->sets, [](const auto &a, const auto &b) {
    return a.second == b.second ? a.first > b.first : a.second > b.second;
  });

  switch (this->sets.size()) {
  case 0:
    // all jokers
    this->value = five_of_a_kind;
    break;
  case 1:
    this->value = five_of_a_kind;
    break;
  case 2: {
    auto [card, count] = sets[0];
    if (count == 4) {
      this->value = four_of_a_kind;
    } else {
      this->value = full_house;
    }
  } break;
  case 3: {
    auto [card, count] = sets[0];
    if (count == 3) {
      this->value = three_of_a_kind;
    } else {
      this->value = two_pair;
    }
  } break;
  case 4:
    this->value = pair;
    break;
  default:
    this->value = high;
  }
}

bool Hand::operator<(const Hand &other) const {
  if (this->value == other.value) {
    return std::ranges::lexicographical_compare(this->hand, other.hand);
  }

  return this->value < other.value;
}

namespace {
namespace grammar {
namespace dsl = lexy::dsl;

template <bool P2> struct card_mapping;

template <> struct card_mapping<false> {
  using enum card_value;
  static constexpr auto table =
      lexy::symbol_table<card_value>
        .map<LEXY_SYMBOL('2')>(two)
        .map<LEXY_SYMBOL('3')>(three)
        .map<LEXY_SYMBOL('4')>(four)
        .map<LEXY_SYMBOL('5')>(five)
        .map<LEXY_SYMBOL('6')>(six)
        .map<LEXY_SYMBOL('7')>(seven)
        .map<LEXY_SYMBOL('8')>(eight)
        .map<LEXY_SYMBOL('9')>(nine)
        .map<LEXY_SYMBOL('T')>(ten)
        .map<LEXY_SYMBOL('J')>(jack)
        .map<LEXY_SYMBOL('Q')>(queen)
        .map<LEXY_SYMBOL('K')>(king)
        .map<LEXY_SYMBOL('A')>(ace) ;
};

template <> struct card_mapping<true> {
  using enum card_value;
  static constexpr auto table =
      lexy::symbol_table<card_value>
        .map<LEXY_SYMBOL('2')>(two)
        .map<LEXY_SYMBOL('3')>(three)
        .map<LEXY_SYMBOL('4')>(four)
        .map<LEXY_SYMBOL('5')>(five)
        .map<LEXY_SYMBOL('6')>(six)
        .map<LEXY_SYMBOL('7')>(seven)
        .map<LEXY_SYMBOL('8')>(eight)
        .map<LEXY_SYMBOL('9')>(nine)
        .map<LEXY_SYMBOL('T')>(ten)
        .map<LEXY_SYMBOL('J')>(joker)
        .map<LEXY_SYMBOL('Q')>(queen)
        .map<LEXY_SYMBOL('K')>(king)
        .map<LEXY_SYMBOL('A')>(ace) ;
};

template <bool P2> struct hand {
  using enum card_value;

  static constexpr auto card = card_mapping<P2>::table;

  static constexpr auto rule = dsl::symbol<card>(dsl::ascii::alnum) +
                               dsl::symbol<card>(dsl::ascii::alnum) +
                               dsl::symbol<card>(dsl::ascii::alnum) +
                               dsl::symbol<card>(dsl::ascii::alnum) +
                               dsl::symbol<card>(dsl::ascii::alnum);

  static constexpr auto value = lexy::construct<Hand>;
};

template <bool P2> struct production {
  static constexpr auto whitespace = dsl::ascii::blank;

  static constexpr auto rule = dsl::p<hand<P2>> + dsl::integer<int>;

  static constexpr auto value = lexy::construct<std::pair<Hand, int>>;
};
} // namespace grammar
} // namespace

long part1(std::istream &input) {
  std::string line;

  std::vector<std::pair<Hand, int>> hands;
  while (std::getline(input, line)) {
    auto str = lexy::string_input(line);
    auto result =
        lexy::parse<grammar::production<false>>(str, lexy_ext::report_error);

    if (!result) {
      throw std::runtime_error(std::format("failed to parse line: {}", line));
    }

    hands.emplace_back(result.value());
  }

  std::ranges::sort(
      hands, [](const auto &a, const auto &b) { return a.first < b.first; });

  long total = 0;
  for (int i = 1; const auto &p : hands) {
    auto [hand, value] = p;
    total += value * i;
    i++;
  }

  return total;
}

long part2(std::istream &input) {
  std::string line;

  std::vector<std::pair<Hand, int>> hands;
  while (std::getline(input, line)) {
    auto str = lexy::string_input(line);
    auto result =
        lexy::parse<grammar::production<true>>(str, lexy_ext::report_error);

    if (!result) {
      throw std::runtime_error(std::format("failed to parse line: {}", line));
    }

    hands.emplace_back(result.value());
  }

  std::ranges::sort(
      hands, [](const auto &a, const auto &b) { return a.first < b.first; });

  long total = 0;
  for (int i = 1; const auto &p : hands) {
    auto [hand, value] = p;
    total += value * i;
    i++;
  }

  return total;
}

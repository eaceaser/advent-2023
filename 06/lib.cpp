#include <lexy/callback.hpp>
#include <lexy/callback/container.hpp>
#include <lexy/dsl.hpp>

#include <algorithm>
#include <bits/ranges_algo.h>
#include <iostream>
#include <lexy/action/parse.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>
#include <ranges>
#include <string>
#include <vector>

namespace {

struct Race {
  int time;
  int distance;
};

namespace grammar {
namespace dsl = lexy::dsl;

struct numbers {
  static constexpr auto rule = dsl::list(dsl::integer<int>);

  static constexpr auto value = lexy::as_list<std::vector<int>>;
};

struct production {
  static constexpr auto rule = [] {
    auto line1 = LEXY_LIT("Time:") + dsl::p<numbers> + dsl::newline;
    auto line2 = LEXY_LIT("Distance:") + dsl::p<numbers>;
    return line1 + line2;
  }();

  static constexpr auto whitespace = dsl::ascii::blank;

  static constexpr auto value = lexy::callback<std::vector<Race>>(
      [](std::vector<int> times, std::vector<int> distances) {
        std::vector<Race> rv;

        std::ranges::transform(times, distances, std::back_inserter(rv),
                               [](auto time, auto distance) {
                                 return Race{time, distance};
                               });

        return rv;
      });
};
} // namespace grammar

namespace grammar2 {
namespace dsl = lexy::dsl;

struct numbers {
  static constexpr auto rule = dsl::list(dsl::integer<long>);

  static constexpr auto value =
      lexy::fold<std::string>("",
                              [](std::string in, long n) {
                                return in + std::to_string(n);
                              }) >>
      lexy::callback<long>([](std::string in) { return std::stol(in); });
};

struct production {
  static constexpr auto rule = [] {
    auto line1 = LEXY_LIT("Time:") + dsl::p<numbers> + dsl::newline;
    auto line2 = LEXY_LIT("Distance:") + dsl::p<numbers>;
    return line1 + line2;
  }();

  static constexpr auto whitespace = dsl::ascii::blank;

  static constexpr auto value = lexy::construct<std::pair<long, long>>;
};
} // namespace grammar2
} // namespace

int part1(std::istream &input) {
  auto it = std::istreambuf_iterator(input);
  std::string in(it, {});
  auto res = lexy::parse<grammar::production>(lexy::string_input(in),
                                              lexy_ext::report_error);
  if (!res.has_value()) {
    throw std::runtime_error("failed to parse");
  }

  auto races = res.value();
  int total = 1;
  for (auto [time, best_distance] : races) {
    int win_count = 0;
    for (int t = 0; t < time; ++t) {
      int speed = t;
      int remaining = time - t;
      int distance = remaining * speed;

      if (distance > best_distance) {
        ++win_count;
      }
    }
    total *= win_count;
  }
  return total;
}

long part2(std::istream &input) {
  auto it = std::istreambuf_iterator(input);
  std::string in(it, {});
  auto res = lexy::parse<grammar2::production>(lexy::string_input(in),
                                               lexy_ext::report_error);
  if (!res.has_value()) {
    throw std::runtime_error("failed to parse");
  }

  long win_count = 0;
  auto [time, best_distance] = res.value();
  for (long t = 0; t < time; ++t) {
    long speed = t;
    long remaining = time - t;
    long distance = remaining * speed;

    if (distance > best_distance) {
      ++win_count;
    }
  }

  return win_count;
}

#include <algorithm>
#include <iostream>
#include <istream>
#include <iterator>
#include <lexy/action/parse.hpp>
#include <lexy/callback/container.hpp>
#include <lexy/callback/string.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/buffer.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <vector>

namespace {
struct MapEntry {
  long destStart;
  long sourceStart;
  long length;
};

struct Mapping {
  std::string source;
  std::string dest;
  std::vector<MapEntry> entries;
};

template <bool P2> struct Almanac;

template <> struct Almanac<true> {
  std::string input;
  std::vector<std::pair<long, long>> inputs;
  std::vector<Mapping> mappings;
};

template <> struct Almanac<false> {
  std::string input;
  std::vector<long> inputs;
  std::vector<Mapping> mappings;
};

namespace grammar {
namespace dsl = lexy::dsl;

struct identifier {
  static constexpr auto rule = dsl::identifier(dsl::ascii::alpha);
  static constexpr auto value = lexy::as_string<std::string>;
};

struct mapEntry {
  static constexpr auto rule = dsl::peek(dsl::digit<>) >>
                               dsl::times<3>(dsl::integer<long>) + dsl::newline;

  static constexpr auto value = lexy::construct<MapEntry>;
};

struct mapBody {
  static constexpr auto rule = dsl::list(dsl::p<mapEntry>);

  static constexpr auto value = lexy::as_list<std::vector<MapEntry>>;
};

struct mapping {
  static constexpr auto rule = [] {
    auto header = dsl::p<identifier> + LEXY_LIT("-to-") + dsl::p<identifier> +
                  LEXY_LIT("map:") + dsl::newline;

    return header + dsl::p<mapBody>;
  }();

  static constexpr auto value = lexy::construct<Mapping>;
};

struct mappings {
  static constexpr auto rule =
      dsl::list(dsl::p<mapping>, dsl::sep(dsl::newline));

  static constexpr auto value = lexy::as_list<std::vector<Mapping>>;
};

struct range {
  static constexpr auto rule =
      dsl::peek(dsl::digit<>) >> dsl::integer<long> + dsl::integer<long>;
  static constexpr auto value = lexy::construct<std::pair<long, long>>;
};

template <bool P2> struct inputs;

template <> struct inputs<true> {
  static constexpr auto rule =
      dsl::list(dsl::p<range>) + dsl::newline + dsl::newline;
  static constexpr auto value =
      lexy::as_list<std::vector<std::pair<long, long>>>;
};

template <> struct inputs<false> {
  static constexpr auto rule =
      dsl::list(dsl::integer<long>) + dsl::newline + dsl::newline;
  static constexpr auto value = lexy::as_list<std::vector<long>>;
};

template <bool P2> struct production {
  static constexpr auto rule =
      dsl::p<identifier> + dsl::colon + dsl::p<inputs<P2>> + dsl::p<mappings>;

  static constexpr auto whitespace = dsl::ascii::blank;
  static constexpr auto value = lexy::construct<Almanac<P2>>;
};
} // namespace grammar
} // namespace

int part1(std::istream &input) {
  auto it = std::istreambuf_iterator(input);
  std::string in(it, {});
  auto res = lexy::parse<grammar::production<false>>(lexy::string_input(in),
                                                     lexy_ext::report_error);
  if (!res.has_value()) {
    throw std::runtime_error("failed to parse");
  }

  auto almanac = res.value();
  std::map<std::string, std::string> categoryPath;
  std::map<std::string, std::map<long, MapEntry>> sourceIndexes;
  for (auto map : almanac.mappings) {
    categoryPath.emplace(map.source, map.dest);

    std::map<long, MapEntry> sourceIndex;
    for (auto entry : map.entries) {
      sourceIndex.emplace(entry.sourceStart, entry);
    }

    sourceIndexes.emplace(map.source, sourceIndex);
  }

  auto startCategory = almanac.input == "seeds" ? "seed" : almanac.input;
  constexpr auto targetCategory = "location";
  auto category = startCategory;

  std::set<std::string> seenCategories;
  auto inputs = almanac.inputs;

  while (category != targetCategory) {
    if (seenCategories.contains(category)) {
      throw std::runtime_error("category " + category + " traversed already");
    }

    auto sourceIndex = sourceIndexes[category];
    for (int i = 0; auto input : inputs) {
      long output = input;

      auto possibleRange = sourceIndex.lower_bound(input);
      if (possibleRange->first == input ||
          possibleRange != sourceIndex.begin()) {
        if (possibleRange->first != input) {
          --possibleRange;
        }
        auto entry = possibleRange->second;
        if (long delta = input - entry.sourceStart; delta < entry.length) {
          // we're in range
          output = entry.destStart + delta;
        }
      }
      inputs[i] = output;
      ++i;
    }

    // walk to next category
    seenCategories.insert(category);
    category = categoryPath[category];
  }

  return *std::ranges::min_element(inputs.begin(), inputs.end());
}

int part2(std::istream &input) {
  auto it = std::istreambuf_iterator(input);
  std::string in(it, {});
  auto res = lexy::parse<grammar::production<true>>(lexy::string_input(in),
                                                    lexy_ext::report_error);
  if (!res.has_value()) {
    throw std::runtime_error("failed to parse");
  }

  auto almanac = res.value();
  std::map<std::string, std::string> categoryPath;

  std::map<std::string, std::map<long, std::pair<long, long>>> indexes;
  for (auto map : almanac.mappings) {
    categoryPath.emplace(map.source, map.dest);
    std::map<long, std::pair<long, long>> sourceIndex;

    long lastEnd = 0;
    std::ranges::sort(map.entries, [](auto a, auto b) {
      return a.sourceStart < b.sourceStart;
    });
    for (auto entry : map.entries) {
      // [last, this)
      if (entry.sourceStart > lastEnd) {
        sourceIndex.emplace(lastEnd,
                            std::make_pair(lastEnd, entry.sourceStart));
      }
      // [this, this end]
      sourceIndex.emplace(
          entry.sourceStart,
          std::make_pair(entry.destStart, entry.destStart + entry.length));

      lastEnd = entry.sourceStart + entry.length;
    }
    // [last, max)
    sourceIndex.emplace(lastEnd, std::make_pair(lastEnd, LONG_MAX));
    indexes.emplace(map.source, sourceIndex);
  }

  std::set<std::string> seenCategories;
  auto startCategory = almanac.input == "seeds" ? "seed" : almanac.input;
  auto category = startCategory;
  constexpr auto targetCategory = "location";

  auto inputs = almanac.inputs;
  for (auto &input : inputs) {
    input.second = input.first + input.second;
  }

  while (category != targetCategory) {
    std::vector<std::pair<long, long>> nextInputs;

    if (seenCategories.contains(category)) {
      throw std::runtime_error("category " + category + " traversed already");
    }

    auto index = indexes[category];
    std::ranges::sort(inputs, [](auto a, auto b) { return a.first < b.first; });
    for (auto input : inputs) {
      auto [inputLeft, inputRight] = input;

      auto startIt = index.lower_bound(inputLeft);
      auto endIt = index.lower_bound(inputRight);

      if (startIt->first != inputLeft || startIt != index.begin()) {
        --startIt;
      }
      // if (endIt->first != inputRight) {
      //   --endIt;
      // }

      for (auto range : std::ranges::subrange(startIt, endIt)) {
        auto [sourceLeft, destRange] = range;
        auto [destLeft, destRight] = destRange;

        // compute destination left bound
        long lowerBound = std::max(sourceLeft, inputLeft);
        long delta = lowerBound - sourceLeft;
        long newDestLeft = destLeft + delta;

        // compute destination right bound
        long length =
            (destRight == LONG_MAX ? inputRight // we're in the source space if
                                                // we're on the trailing range
                                   : destRight) -
            destLeft;

        if (length > 0) {
          long sourceRight = sourceLeft + length;
          long upperBound = std::min(sourceRight, inputRight);

          long rightDelta = upperBound - sourceLeft;
          long newDestRight = destLeft + rightDelta;

          nextInputs.emplace_back(newDestLeft, newDestRight);
        }
      }
    }

    // walk to next category
    seenCategories.insert(category);
    category = categoryPath[category];
    inputs = nextInputs;
  }

  std::ranges::sort(inputs, [](auto a, auto b) { return a.first < b.first; });
  return inputs[0].first;
}

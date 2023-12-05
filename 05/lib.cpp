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

struct Almanac {
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

struct inputs {
  static constexpr auto rule =
      dsl::list(dsl::integer<long>) + dsl::newline + dsl::newline;

  static constexpr auto value = lexy::as_list<std::vector<long>>;
};

struct production {
  static constexpr auto rule =
      dsl::p<identifier> + dsl::colon + dsl::p<inputs> + dsl::p<mappings>;

  static constexpr auto whitespace = dsl::ascii::blank;
  static constexpr auto value = lexy::construct<Almanac>;
};
} // namespace grammar
} // namespace

long part1(std::istream &input) {
  auto it = std::istreambuf_iterator(input);
  std::string in(it, {});
  auto res = lexy::parse<grammar::production>(lexy::string_input(in),
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

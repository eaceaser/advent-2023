#include <array>
#include <bits/ranges_algobase.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <vector>

int main() {
  size_t row_length = 0;
  size_t cols = 0;

  std::vector<char> schematic;
  std::vector<size_t> gears;

  std::ifstream input_file("input");
  if (input_file.is_open()) {
    std::string line;
    while (getline(input_file, line)) {
      if (row_length == 0) {
        row_length = line.length();
      }

      std::vector<char> row(row_length);
      int i = 0;
      for (auto c : line) {
        if (c == '*') {
          size_t pos = cols * row_length + i;
          gears.push_back(pos);
        } else if (isdigit(c)) {
          row[i] = c;
        } else {
          row[i] = 0;
        }
        ++i;
      }

      schematic.insert(schematic.end(), row.begin(), row.end());
      ++cols;
    }
  }

  auto pos = [row_length](size_t x, size_t y) { return y * row_length + x; };
  auto coords = [row_length](size_t pos) {
    return std::pair(pos % row_length, pos / row_length);
  };

  int sum = 0;
  for (auto gear : gears) {
    auto [x, y] = coords(gear);

    // part locations appear to never be on edges, can fudge boundary math
    std::array num_positions = {pos(x - 1, y - 1), pos(x, y - 1),
                                pos(x + 1, y - 1), pos(x - 1, y),
                                pos(x + 1, y),     pos(x - 1, y + 1),
                                pos(x, y + 1),     pos(x + 1, y + 1)};

    size_t last_x = 0, last_y = 0;
    int found = 0;
    int ratio = 1;
    for (auto pos : num_positions) {
      auto [x, y] = coords(pos);

      // reset if we've changed rows
      if (y - 1 == last_y) {
        last_x = 0;
      }

      if (schematic[pos] != 0) {
        // check to see if we've continguously matched a number
        if (x - 1 == last_x) {
          last_x = x;
          continue;
        }

        auto next_row = (y + 1) * row_length;
        auto fwd_view =
            std::ranges::subrange(schematic.begin() + pos,
                                  schematic.begin() + next_row) |
            std::views::take_while([](char c) { return isdigit(c); });

        auto rev_view =
            std::ranges::subrange(schematic.rbegin() + (schematic.size() - pos),
                                  schematic.rbegin() + (schematic.size() - y)) |
            std::views::take_while([](char c) { return isdigit(c); });

        std::stringstream num;
        std::ranges::copy(rev_view | std::views::reverse,
                          std::ostream_iterator<char>(num, ""));
        std::ranges::copy(fwd_view, std::ostream_iterator<char>(num, ""));

        last_x = x;
        ratio *= std::stoi(num.str());
        ++found;
      }

      last_y = y;
    }

    if (found == 2) {
      sum += ratio;
    }
  }

  std::cout << sum << std::endl;
}

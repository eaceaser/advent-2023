#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy/action/parse.hpp>
#include <lexy/action/trace.hpp>
#include <lexy/callback/fold.hpp>
#include <lexy/callback/container.hpp>
#include <lexy_ext/report_error.hpp>

namespace {
    struct Round {
        int red;
        int blue;
        int green;
    };

    struct Game {
        int id;
        std::vector<Round> rounds;
    };

    namespace grammar {
        namespace dsl = lexy::dsl;

        struct round {
            static constexpr auto entities = lexy::symbol_table<char>
                    .map<LEXY_SYMBOL("red")>('r')
                    .map<LEXY_SYMBOL("green")>('g')
                    .map<LEXY_SYMBOL("blue")>('b');

            static constexpr auto rule = [] {
                return dsl::list(
                    dsl::integer<int>(dsl::digits<>) + dsl::ascii::space + dsl::symbol<entities>(
                        dsl::identifier(dsl::ascii::alpha)), dsl::sep(LEXY_LIT(", ")));
            }();

            static constexpr auto value = lexy::fold_inplace<Round>(Round{0, 0, 0}, [](Round& round, int n, char kw) {
                switch (kw) {
                    case 'r':
                        round.red = n;
                        break;
                    case 'g':
                        round.green = n;
                        break;
                    case 'b':
                        round.blue = n;
                        break;
                }
            });
        };

        struct game {
            static constexpr auto rule = dsl::list(dsl::p<round>, dsl::sep(LEXY_LIT("; ")));
            static constexpr auto value = lexy::as_list<std::vector<Round>>;
        };

        struct production {
            static constexpr auto rule = [] {
                auto header = LEXY_LIT("Game") + dsl::ascii::space + dsl::integer<int>(dsl::digits<>);
                return header + dsl::colon + dsl::ascii::space + dsl::p<game> + dsl::eof;
            }();

            static constexpr auto value = lexy::construct<Game>;
        };
    }
}

bool is_possible(const Game& game) {
    constexpr int total_red = 12;
    constexpr int total_green = 13;
    constexpr int total_blue = 14;

    return std::ranges::all_of(game.rounds.cbegin(), game.rounds.cend(), [](auto round) {
        return round.red <= total_red && round.green <= total_green && round.blue <= total_blue;
    });
}

int main() {
    std::ifstream input_file("input");

    if (input_file.is_open()) {
        int sum = 0;
        std::string line;
        while (getline(input_file, line)) {
            auto str = lexy::string_input(line);
            auto result = lexy::parse<grammar::production>(str, lexy_ext::report_error);
            if (result.has_value()) {
                auto game = result.value();
                if (is_possible(game)) {
                    std::cout << "possible game " << game.id << std::endl;
                    sum += game.id;
                } else {
                    std::cout << "impossible game " << game.id << std::endl;
                }
            } else {
                std::cout << "bad line: " << line << std::endl;
            }
        }

        std::cout << sum << std::endl;
    }
}

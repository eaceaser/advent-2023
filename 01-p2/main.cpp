#include <fstream>
#include <vector>
#include <array>
#include <iostream>
#include <optional>

struct data {
    int digit;
    int pos;
    bool end;
};

template<std::size_t N>
std::optional<int> update_state(std::array<data, N> matches, std::array<int, 9> &state) {
    std::array<bool, 9> seen{};
    seen.fill(false);

    for (auto match : matches) {
        int prev = state[match.digit-1];

        if (match.pos-1 == prev) {
            if (match.end) {
                return std::optional<int>(match.digit);
            }

            ++state[match.digit-1];
            seen[match.digit-1] = true;
        } else if (match.pos == 0 && prev == 0) {
            seen[match.digit-1] = true;
        }
    }

    for (int i=0; i<10; ++i) {
        if (!seen[i]) {
            state[i] = -1;
        }
    }

    return std::nullopt;
}

/*
 * one
 * two
 * three
 * four
 * five
 * six
 * seven
 * eight
 * nine
 **/

template<char C, bool R>
constexpr auto lookup_char() {
    if constexpr (C == 'o') {
        return std::array<data, 3>{{
            {1, R ? 2 : 0, R ? true : false },
            {2, R ? 0 : 2, R ? false : true },
            {4, R ? 2 : 1, false },
        }};
    } else if constexpr (C == 'n') {
        return std::array<data, 4>{{
            {1, 1, false},
            {7, R ? 0 : 4, R ? false : true},
            {9, R ? 3 : 0, R ? true : false},
            {9, R ? 1 : 2, false},
        }};
    } else if constexpr (C == 'e') {
        return std::array<data, 8>{{
            {1, R ? 0 : 2, R ? false : true},
            {3, R ? 1 : 3, false},
            {3, R ? 0 : 4, R ? false : true},
            {5, R ? 0 : 3, R ? false : true},
            {7, R ? 3 : 1, false},
            {7, R ? 1 : 3, false},
            {8, R ? 4 : 0, R ? true : false},
            {9, R ? 0 : 3, R ? false : true},
        }};
    } else if constexpr (C == 't') {
        return std::array<data, 3>{{
            {2, R ? 2 : 0, R ? true : false},
            {3, R ? 4 : 0, R ? true : false},
            {8, R ? 0 : 4, R ? false : true},
        }};
    } else if constexpr (C == 'w') {
        return std::array<data, 1>{{
            {2, 1, false},
        }};
    } else if constexpr (C == 'g') {
        return std::array<data, 1>{{
            {8, 2, false},
        }};
    } else if constexpr (C == 'h') {
        return std::array<data, 2>{{
            {3, R ? 3 : 1, false},
            {8, R ? 1 : 3, false},
        }};
    } else if constexpr (C == 'r') {
        return std::array<data, 2>{{
            {3, 2, false},
            {4, R ? 0 : 3, R ? false : true},
        }};
    } else if constexpr (C == 'f') {
        return std::array<data, 2>{{
            {4, R ? 3 : 0, R ? true : false},
            {5, R ? 3 : 0, R ? true : false},
        }};
    } else if constexpr (C == 'u') {
        return std::array<data, 1>{{
            {4, R ? 1 : 2, false},
        }};
    } else if constexpr (C == 'i') {
        return std::array<data, 4>{{
            {5, R ? 2 : 1, false},
            {6, 1, false},
            {8, R ? 3 : 1, false},
            {9, R ? 2 : 1, false},
        }};
    } else if constexpr (C == 'v') {
        return std::array<data, 2>{{
            {5, R ? 1 : 2, false },
            {7, 2, false },
        }};
    } else if constexpr (C == 's') {
        return std::array<data, 2>{{
            { 6, R ? 2 : 0, R ? true : false },
            { 7, R ? 4 : 0, R ? true : false },
        }};
    } else if constexpr (C == 'x') {
        return std::array<data, 2>{{
            { 6, R ? 0 : 2, R ? false : true },
        }};
    } else {
        return std::array<data, 0>{{}};
    }
}

template <bool R = false>
std::optional<int> parse(char c, std::array<int, 9> &state) {
    switch (c) {
        case 'o':
            return update_state(lookup_char<'o', R>(), state);
        case 'n':
            return update_state(lookup_char<'n', R>(), state);
        case 'e':
            return update_state(lookup_char<'e', R>(), state);
        case 't':
            return update_state(lookup_char<'t', R>(), state);
        case 'w':
            return update_state(lookup_char<'w', R>(), state);
        case 'h':
            return update_state(lookup_char<'h', R>(), state);
        case 'g':
            return update_state(lookup_char<'g', R>(), state);
        case 'r':
            return update_state(lookup_char<'r', R>(), state);
        case 'f':
            return update_state(lookup_char<'f', R>(), state);
        case 'u':
            return update_state(lookup_char<'u', R>(), state);
        case 'i':
            return update_state(lookup_char<'i', R>(), state);
        case 'v':
            return update_state(lookup_char<'v', R>(), state);
        case 's':
            return update_state(lookup_char<'s', R>(), state);
        case 'x':
            return update_state(lookup_char<'x', R>(), state);
        default:
            state.fill(-1);
            return std::nullopt;
    }
}

char num_to_c(int i) {
    return '0' + i;
}

int main() {
    std::ifstream inputFile("input");
    if (inputFile.is_open()) {
        std::string line;
        int sum = 0;

        while (getline(inputFile, line)) {
            std::array<int, 9> state;
            state.fill(-1);
            std::pair<char, char> nums;

            for (auto c : line) {
                if (isdigit(c)) {
                    nums.first = c;
                    break;
                }

                auto parsed = parse(c, state);
                if (parsed.has_value()) {
                    nums.first = num_to_c(parsed.value());
                    break;
                }
            }

            state.fill(-1);
            for (auto c = line.rbegin(); c != line.rend(); ++c) {
                if (isdigit(*c)) {
                    nums.second = *c;
                    break;
                }

                auto parsed = parse<true>(*c, state);
                if (parsed.has_value()) {
                    nums.second = num_to_c(parsed.value());
                    break;
                }
            }

            std::string num = {nums.first, nums.second};
            sum += std::stoi(num);
            std::cout << line << " : " << nums.first << " " << nums.second << " " << num << " " << sum << std::endl;
        }

        std::cout << sum << std::endl;
        inputFile.close();
    }

    return 0;
}


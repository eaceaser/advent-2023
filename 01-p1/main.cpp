#include <iostream>
#include <fstream>

int main() {
    std::ifstream inputFile("input");

    if (inputFile.is_open()) {
        std::string line;
        int sum = 0;

        while (getline(inputFile, line)) {
            std::pair<char, char> nums;

            for (auto c : line) {
                if (isdigit(c)) {
                    nums.first = c;
                    break;
                }
            }
            for (auto c = line.rbegin(); c != line.rend(); ++c) {
                if (isdigit(*c)) {
                    nums.second = *c;
                    break;
                }
            }

            std::string num = {nums.first, nums.second};
            sum += std::stoi(num);
        }

        std::cout << sum << std::endl;
        inputFile.close();
    }

    return 0;
}

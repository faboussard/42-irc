#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string removeQuotes(const std::string& str) {
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), '\"'), result.end());
    return result;
}

void extractCodeIcon(const json& j, std::ofstream& outputFile) {
    for (const auto& item : j) {
        if (item.contains("code") && item.contains("day")) {
            std::string day = removeQuotes(item["day"].dump());
            outputFile << item["code"] << ": " << day << '\n';
        } else {
            std::cerr << "Missing code or icon in item: " << item << std::endl;
        }
    }
}

int main(void) {
    std::ifstream inputFile("weather_code.json");
    if (!inputFile.is_open()) {
        std::cerr << "Could not open the file" << std::endl;
        return 1;
    }
    std::ofstream outputFile("weather_code.txt");
    if (!outputFile.is_open()) {
        std::cerr << "Could not open the file" << std::endl;
        return 1;
    }
    json j;
    inputFile >> j;
    extractCodeIcon(j, outputFile);
    return (0);
}

// g++ -o extract_code extract_code.cpp -std=c++11

// Created by: Jason Erwin Clyde V. Dimalanta (ID: 12198331)

#include "main.h"

int main() {
    // Create an unordered map to store the config values
    std::unordered_map<std::string, long long int> configValues;
    printHeader();

    // Read the config file
    try {
        readConfig("config.txt", configValues);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Print the config values
    printConfig(configValues);
    
    // Initialize the matchmaker
    Matchmaker& matchmaker = Matchmaker::getInstance(&configValues);

    matchmaker.startMatchmaking();
    ThreadPool::getInstance(configValues["n"] + 1).wait();
    return 0;
}

void printHeader() {
    std::cout << "Welcome to the Dungeon Matchmaking program!" << std::endl;
}

void readConfig(std::string fileName, std::unordered_map<std::string, long long int>& configValues) {
    // Open file in read mode
    std::ifstream file(fileName, std::ifstream::in);

    // Check if the file was opened successfully
    if (!file) {
        throw std::invalid_argument("Could not open file: " + fileName);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        int posDelimiter = line.find("=");
        
        // Get the key and value from the line
        std::string key = trim(line.substr(0, posDelimiter));
        std::string value = trim(line.substr(posDelimiter + 1, line.length()));
        
        // Check if the value is a valid non-negative number
        if(std::all_of(value.begin(), value.end(), ::isdigit)) {
            long long int num = std::stoi(value);
            configValues.emplace(key, num);
        } else {
            throw std::invalid_argument("Invalid value in config file: " + value);
        }
    }
}

// Trim leading and trailing whitespaces from a string
std::string trim(const std::string& str){
    std::string trimmed = str;
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), trimmed.end());
    return trimmed;
}

void printConfig(std::unordered_map<std::string, long long int>& configValues) {
    std::cout << "Config values:" << std::endl;
    for (auto& pair : configValues) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}

void testThreadPool(std::unordered_map<std::string, long long int>& configValues) {
    ThreadPool& pool = ThreadPool::getInstance(configValues["n"]);
    for (int i = 0; i < 10; i++) {
        pool.enqueue([i](){
            std::this_thread::sleep_for(std::chrono::seconds(10));
            std::cout << "Task " << i << " completed" << std::endl;
        });
    }
}

#include <iostream>
#include <filesystem>
#include "Application.hpp"

int main() {
    try {
        std::filesystem::create_directories("data");

        std::cout << "WiseVault - Bank Management System" << std::endl;
        std::cout << "Genesis Team 10" << std::endl;

        Application app;
        app.run();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
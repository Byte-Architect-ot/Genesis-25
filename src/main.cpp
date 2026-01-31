#include <iostream>
#include <filesystem>
#include "Application.hpp"

int main() {
    try {
        // Create data directory if it doesn't exist
        std::filesystem::create_directories("data");
        
        std::cout << "                                     " << std::endl;
        std::cout << "    WiseVault" << std::endl;
        std::cout << "                                     " << std::endl;
        
        // Create and run application
        Application app;
        app.run();
        
        std::cout << "Thank you!" << std::endl;
        std::cout << "Genesis Team 10 - Prithvi, Shreyash, Shreenika, Atharv" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
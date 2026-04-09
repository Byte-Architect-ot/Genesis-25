#include "Application.hpp"
#include <filesystem>

int main() {
    std::filesystem::create_directories("data");

    Application app;
    app.run();

    return 0;
}
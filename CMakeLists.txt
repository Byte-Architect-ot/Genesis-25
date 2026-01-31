cmake_minimum_required(VERSION 3.16)
project(SimpleBankSystem)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML library
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Download ImGui
include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.9
)
FetchContent_MakeAvailable(imgui)

# Download ImGui-SFML
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG 2.6.x
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_MakeAvailable(imgui-sfml)

# Source files
set(SOURCES
    src/main.cpp
    src/Account.cpp
    src/User.cpp
    src/Database.cpp
    src/Application.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
    SQLite::SQLite3
)

# Create data folder
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)
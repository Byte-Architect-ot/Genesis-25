cmake_minimum_required(VERSION 3.16)
project(WiseVault)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)

add_executable(WiseVault
    src/main.cpp
    src/Application.cpp
    src/BankSystem.cpp
    src/Database.cpp
    src/User.cpp
    src/Account.cpp
    src/Transaction.cpp
    src/Loan.cpp
)

target_include_directories(WiseVault PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_link_libraries(WiseVault PRIVATE
    SQLite::SQLite3
)
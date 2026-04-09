cmake_minimum_required(VERSION 3.16)
project(WiseVault)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WiseVault
    src/main.cpp
    src/User.cpp
    src/Account.cpp
    src/Transaction.cpp
    src/Loan.cpp
    src/Database.cpp
    src/BankSystem.cpp
    src/Application.cpp
)

target_include_directories(WiseVault PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
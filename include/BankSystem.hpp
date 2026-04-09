#ifndef BANKSYSTEM_HPP
#define BANKSYSTEM_HPP

#include "Database.hpp"
#include "User.hpp"
#include <optional>
#include <string>

class BankSystem {
private:
    Database db;
    std::optional<User> currentUser;

    std::string profilesFile = "data/profiles.txt";
    std::string accountsFile = "data/accounts.txt";
    std::string transactionsFile = "data/transactions.txt";
    std::string loansFile = "data/loans.txt";

private:
    static int readInt(const std::string& prompt, int minVal, int maxVal);
    static double readDouble(const std::string& prompt, double minVal);
    static std::string readLine(const std::string& prompt);
    static std::string nowDateTime();

    void mainMenu();
    void registerMenu();
    void loginMenu();

    void adminMenu();
    void userMenu();

public:
    BankSystem();
    void run();
};

#endif
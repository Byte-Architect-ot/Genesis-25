#ifndef BANKSYSTEM_HPP
#define BANKSYSTEM_HPP

#include "Database.hpp"
#include <optional>

class BankSystem {
private:
    Database db;
    std::optional<User> currentUser;

private:
    static int readInt(const std::string& prompt, int minVal, int maxVal);
    static double readDouble(const std::string& prompt, double minVal);
    static std::string readLine(const std::string& prompt);
    static std::string toUpper(std::string s);

    void mainMenu();
    void login();
    void registerUser();

    void adminMenu();
    void userMenu();

    void adminViewUsers();
    void adminViewAccounts();
    void adminViewAccountTransactions();

    void userListAccounts();
    void userOpenAccount();
    void userDeposit();
    void userWithdraw();
    void userTransfer();
    void userTransactions();

    bool pickUserAccount(std::unique_ptr<Account>& outAccount) const;

public:
    BankSystem();
    void run();
};

#endif
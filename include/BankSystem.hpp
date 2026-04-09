#ifndef BANKSYSTEM_HPP
#define BANKSYSTEM_HPP

#include "Database.hpp"
#include <string>

class BankSystem {
private:
    Database db;
    User* currentUser;

private:
    static std::string nowDateTime();
    static int readInt(const std::string& prompt, int minVal, int maxVal);
    static double readDouble(const std::string& prompt, double minVal);
    static std::string readString(const std::string& prompt);

    static std::string normalizeRole(const std::string& r);

    void startMenu();
    void loginMenu();
    void registerMenu();

    void adminMenu();
    void userMenu();

    void adminViewUsers();
    void adminViewAllLoans();
    void adminUpdateLoan();

    void userDeposit();
    void userWithdraw();
    void userViewAccounts();
    void userViewProfile();
    void userViewTransactions();
    void userApplyLoan();
    void userViewLoans();

    Account* pickAccountFromUser() const;

public:
    BankSystem();
    ~BankSystem();

    void run();
};

#endif
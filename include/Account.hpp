#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "User.hpp"
#include <string>

class Account : public User {
private:
    int accountId;
    std::string accountNumber;
    std::string accountType;
    double balance;

public:
    Account(const User& u, int accountId, const std::string& accNo, const std::string& type, double bal);
    virtual ~Account() = default;

    int getAccountId() const;
    const std::string& getAccountNumber() const;
    const std::string& getAccountType() const;

    double getBalance() const;
    void setBalance(double v);

    void deposit(double amount);
    bool withdraw(double amount);

    virtual bool canWithdraw(double amount) const = 0;
};

class SavingsAccount : public Account {
public:
    SavingsAccount(const User& u, int accountId, const std::string& accNo, double bal);
    bool canWithdraw(double amount) const override;
};

class CurrentAccount : public Account {
private:
    double overdraftLimit;

public:
    CurrentAccount(const User& u, int accountId, const std::string& accNo, double bal, double limit = 5000.0);
    bool canWithdraw(double amount) const override;
};

#endif
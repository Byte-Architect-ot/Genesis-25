#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "User.hpp"
#include <string>

class Account : public User {
protected:
    int accountId;
    int userId;
    std::string accountNumber;
    std::string accountType;
    double balance;

public:
    Account(int userId,
            const std::string& username,
            const std::string& fullName,
            const std::string& accountType);

    virtual ~Account() = default;

    int getId() const;
    void setId(int newId);

    int getUserId() const;

    const std::string& getAccountNumber() const;
    void setAccountNumber(const std::string& v);

    const std::string& getAccountType() const;

    double getBalance() const;
    void setBalance(double v);

    void deposit(double amount);
    bool withdraw(double amount);

    virtual bool canWithdraw(double amount) const = 0;
    virtual void calculateInterest() {}
};

class SavingsAccount : public Account {
public:
    SavingsAccount(int userId, const std::string& username, const std::string& fullName);
    bool canWithdraw(double amount) const override;
    void calculateInterest() override;
};

class CurrentAccount : public Account {
public:
    CurrentAccount(int userId, const std::string& username, const std::string& fullName);
    bool canWithdraw(double amount) const override;
};

class FixedDepositAccount : public Account {
public:
    FixedDepositAccount(int userId, const std::string& username, const std::string& fullName);
    bool canWithdraw(double amount) const override;
    void calculateInterest() override;
};

#endif
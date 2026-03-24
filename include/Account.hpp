#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <string>

// Base class - all account types inherit from this
class Account {
protected:
    int id;
    int userId;
    std::string accountNumber;
    std::string accountType;
    double balance;

public:
    Account(int userId, const std::string& type);
    virtual ~Account();

    // Getters
    int getId() const;
    int getUserId() const;
    std::string getAccountNumber() const;
    std::string getAccountType() const;
    double getBalance() const;

    // Setters
    void setId(int newId);
    void setAccountNumber(const std::string& num);
    void setBalance(double newBalance);

    // Virtual - child classes override these for custom rules
    virtual bool canWithdraw(double amount) const;
    virtual double getMinimumBalance() const;

    bool deposit(double amount);
    bool withdraw(double amount);

    static std::string generateAccountNumber();
};

// Savings: must maintain Rs.500 minimum balance
class SavingsAccount : public Account {
private:
    static constexpr double MIN_BALANCE = 500.0;

public:
    SavingsAccount(int userId);
    bool canWithdraw(double amount) const override;
    double getMinimumBalance() const override;
};

// Current: allows overdraft (can go negative up to limit)
class CurrentAccount : public Account {
private:
    double overdraftLimit;

public:
    CurrentAccount(int userId, double overdraft = 1000.0);
    bool canWithdraw(double amount) const override;
    double getOverdraftLimit() const;
};

#endif
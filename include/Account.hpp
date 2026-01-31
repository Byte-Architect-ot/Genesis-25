#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <string>
#include <vector>

// Structure to store transaction details
struct Transaction {
    int id;
    std::string type;      // "DEPOSIT", "WITHDRAW", "TRANSFER_IN", "TRANSFER_OUT"
    double amount;
    double balanceAfter;
    std::string date;
    std::string description;
};

// Base class for all account types
class Account {
protected:
    int id;
    int userId;
    std::string accountNumber;
    std::string accountType;
    double balance;
    std::vector<Transaction> transactions;

public:
    // Constructor
    Account(int userId, const std::string& type);
    
    // Virtual destructor (important for inheritance)
    virtual ~Account() = default;

    // Getters (to access private data)
    int getId() const { return id; }
    int getUserId() const { return userId; }
    std::string getAccountNumber() const { return accountNumber; }
    std::string getAccountType() const { return accountType; }
    double getBalance() const { return balance; }
    std::vector<Transaction>& getTransactions() { return transactions; }

    // Setters (to modify private data)
    void setId(int newId) { id = newId; }
    void setAccountNumber(const std::string& num) { accountNumber = num; }
    void setBalance(double newBalance) { balance = newBalance; }

    // Virtual functions (can be overridden by child classes)
    virtual bool canWithdraw(double amount) const;
    virtual double getMinimumBalance() const { return 0; }
    
    // Common operations
    bool deposit(double amount);
    bool withdraw(double amount);
    
    // Generate unique account number
    static std::string generateAccountNumber();
};

// Savings Account - inherits from Account
class SavingsAccount : public Account {
private:
    static constexpr double MIN_BALANCE = 500.0; 
    
public:
    SavingsAccount(int userId);
    
    // Override parent class functions
    bool canWithdraw(double amount) const override;
    double getMinimumBalance() const override { return MIN_BALANCE; }
};

// Current Account - inherits from Account
class CurrentAccount : public Account {
private:
    double overdraftLimit;  // Can go negative up to this limit
    
public:
    CurrentAccount(int userId, double overdraft = 1000.0);
    
    // Override parent class functions
    bool canWithdraw(double amount) const override;
    double getOverdraftLimit() const { return overdraftLimit; }
};

#endif
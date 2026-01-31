#include "Account.hpp"
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

//THe base class of the account is implemented here
Account::Account(int userId, const std::string& type) 
    : id(0), userId(userId), accountType(type), balance(0.0) {
    // Generate a unique account number when creating new account
    accountNumber = generateAccountNumber();
}

std::string Account::generateAccountNumber() {
    // Create random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 9);
    
    // Generate account number like "ACC1234567890"
    std::stringstream ss;
    ss << "ACC";
    for (int i = 0; i < 10; i++) {
        ss << dis(gen);
    }
    return ss.str();
}

bool Account::canWithdraw(double amount) const {
    return balance >= amount;
}

bool Account::deposit(double amount) {
    if (amount <= 0) {
        return false;
    }
    
    // Add to balance
    balance += amount;
    
    // Create transaction record
    Transaction txn;
    txn.type = "DEPOSIT";
    txn.amount = amount;
    txn.balanceAfter = balance;
    txn.description = "Cash deposit";
    
    // Get current date/time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    txn.date = ss.str();
    
    transactions.push_back(txn);
    
    return true;
}

bool Account::withdraw(double amount) {
    // Check if amount is valid
    if (amount <= 0) {
        return false;
    }
    
    // Check if withdrawal is allowed
    if (!canWithdraw(amount)) {
        return false;
    }
    
    // Subtract from balance
    balance -= amount;
    
    // Create transaction record
    Transaction txn;
    txn.type = "WITHDRAW";
    txn.amount = amount;
    txn.balanceAfter = balance;
    txn.description = "Cash withdrawal";
    
    // Get current date/time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    txn.date = ss.str();
    
    transactions.push_back(txn);
    
    return true;
}

//SaVing account implementation
SavingsAccount::SavingsAccount(int userId) 
    : Account(userId, "SAVINGS") {
    // Savings account starts with minimum balance
    balance = MIN_BALANCE;
}

bool SavingsAccount::canWithdraw(double amount) const {
    // Must maintain minimum balance after withdrawal
    return (balance - amount) >= MIN_BALANCE;
}

//current account 
CurrentAccount::CurrentAccount(int userId, double overdraft) 
    : Account(userId, "CURRENT"), overdraftLimit(overdraft) {
    balance = 0.0;
}

bool CurrentAccount::canWithdraw(double amount) const {
    // Can go negative up to overdraft limit
    return (balance - amount) >= -overdraftLimit;
}
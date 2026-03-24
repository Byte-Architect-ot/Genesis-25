#include "Account.hpp"
#include <random>
#include <sstream>
#include <iostream>

Account::Account(int userId, const std::string& type)
    : id(0), userId(userId), accountType(type), balance(0.0) {
    accountNumber = generateAccountNumber();
}

Account::~Account() {
    // Base class destructor - virtual so child destructors run first
}

int Account::getId() const { return id; }
int Account::getUserId() const { return userId; }
std::string Account::getAccountNumber() const { return accountNumber; }
std::string Account::getAccountType() const { return accountType; }
double Account::getBalance() const { return balance; }

void Account::setId(int newId) { id = newId; }
void Account::setAccountNumber(const std::string& num) { accountNumber = num; }
void Account::setBalance(double newBalance) { balance = newBalance; }

bool Account::canWithdraw(double amount) const {
    return balance >= amount;
}

double Account::getMinimumBalance() const {
    return 0.0;
}

bool Account::deposit(double amount) {
    if (amount <= 0) return false;
    balance += amount;
    return true;
}

// Uses virtual canWithdraw() - polymorphism picks the right child version
bool Account::withdraw(double amount) {
    if (amount <= 0) return false;
    if (!canWithdraw(amount)) return false;
    balance -= amount;
    return true;
}

std::string Account::generateAccountNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 9);

    std::stringstream ss;
    ss << "ACC";
    for (int i = 0; i < 10; i++) {
        ss << dis(gen);
    }
    return ss.str();
}

// Savings - must keep at least Rs.500 after withdrawal
SavingsAccount::SavingsAccount(int userId)
    : Account(userId, "SAVINGS") {
    balance = MIN_BALANCE;
}

bool SavingsAccount::canWithdraw(double amount) const {
    return (balance - amount) >= MIN_BALANCE;
}

double SavingsAccount::getMinimumBalance() const {
    return MIN_BALANCE;
}

// Current - can go negative up to overdraft limit
CurrentAccount::CurrentAccount(int userId, double overdraft)
    : Account(userId, "CURRENT"), overdraftLimit(overdraft) {
    balance = 0.0;
}

bool CurrentAccount::canWithdraw(double amount) const {
    return (balance - amount) >= -overdraftLimit;
}

double CurrentAccount::getOverdraftLimit() const {
    return overdraftLimit;
}
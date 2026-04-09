#include "Account.hpp"

Account::Account(int userId,
                 const std::string& username,
                 const std::string& fullName,
                 const std::string& accountType)
    : accountId(0),
      userId(userId),
      accountNumber(""),
      accountType(accountType),
      balance(500.0) {
    setUsername(username);
    setFullName(fullName);
    setRole("USER");
}

int Account::getId() const { return accountId; }
void Account::setId(int newId) { accountId = newId; }

int Account::getUserId() const { return userId; }

const std::string& Account::getAccountNumber() const { return accountNumber; }
void Account::setAccountNumber(const std::string& v) { accountNumber = v; }

const std::string& Account::getAccountType() const { return accountType; }

double Account::getBalance() const { return balance; }
void Account::setBalance(double v) { balance = v; }

void Account::deposit(double amount) {
    if (amount > 0) balance += amount;
}

bool Account::withdraw(double amount) {
    if (!canWithdraw(amount)) return false;
    balance -= amount;
    return true;
}

SavingsAccount::SavingsAccount(int userId, const std::string& username, const std::string& fullName)
    : Account(userId, username, fullName, "SAVINGS") {}

bool SavingsAccount::canWithdraw(double amount) const {
    if (amount <= 0) return false;
    return amount <= balance;
}

void SavingsAccount::calculateInterest() {
    balance += balance * 0.04;
}

CurrentAccount::CurrentAccount(int userId, const std::string& username, const std::string& fullName)
    : Account(userId, username, fullName, "CURRENT") {}

bool CurrentAccount::canWithdraw(double amount) const {
    if (amount <= 0) return false;
    return amount <= balance;
}

FixedDepositAccount::FixedDepositAccount(int userId, const std::string& username, const std::string& fullName)
    : Account(userId, username, fullName, "FD") {}

bool FixedDepositAccount::canWithdraw(double amount) const {
    (void)amount;
    return false;
}

void FixedDepositAccount::calculateInterest() {
    balance += balance * 0.07;
}
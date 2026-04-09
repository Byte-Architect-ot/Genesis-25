#include "Account.hpp"

Account::Account(int userId,
                 const std::string& username,
                 const std::string& fullName,
                 const std::string& accountType)
    : accountId(0),
      userId(userId),
      accountNumber(""),
      accountType(accountType),
      balance(0.0) {
    setUsername(username);
    setFullName(fullName);
    setRole("USER");
}

int Account::getAccountId() const { return accountId; }
void Account::setAccountId(int v) { accountId = v; }

int Account::getUserId() const { return userId; }
void Account::setUserId(int v) { userId = v; }

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

CurrentAccount::CurrentAccount(int userId, const std::string& username, const std::string& fullName, double overdraftLimit)
    : Account(userId, username, fullName, "CURRENT"),
      overdraftLimit(overdraftLimit) {}

bool CurrentAccount::canWithdraw(double amount) const {
    if (amount <= 0) return false;
    return (balance - amount) >= (-overdraftLimit);
}

double CurrentAccount::getOverdraftLimit() const { return overdraftLimit; }
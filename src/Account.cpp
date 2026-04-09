#include "Account.hpp"

Account::Account(const User& u, int accountId, const std::string& accNo, const std::string& type, double bal)
    : accountId(accountId), accountNumber(accNo), accountType(type), balance(bal) {
    setId(u.getId());
    setUsername(u.getUsername());
    setFullName(u.getFullName());
    setRole(u.getRole());
    setMobile(u.getMobile());
    setEmail(u.getEmail());
    setAddress(u.getAddress());
    setAadhaar(u.getAadhaar());
}

int Account::getAccountId() const { return accountId; }
const std::string& Account::getAccountNumber() const { return accountNumber; }
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

SavingsAccount::SavingsAccount(const User& u, int accountId, const std::string& accNo, double bal)
    : Account(u, accountId, accNo, "SAVINGS", bal) {}

bool SavingsAccount::canWithdraw(double amount) const {
    if (amount <= 0) return false;
    return amount <= getBalance();
}

CurrentAccount::CurrentAccount(const User& u, int accountId, const std::string& accNo, double bal, double limit)
    : Account(u, accountId, accNo, "CURRENT", bal), overdraftLimit(limit) {}

bool CurrentAccount::canWithdraw(double amount) const {
    if (amount <= 0) return false;
    return (getBalance() - amount) >= (-overdraftLimit);
}
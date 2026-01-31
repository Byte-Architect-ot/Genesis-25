#include "User.hpp"
#include <sstream>
#include <iomanip>

//we have done simple password hashing here (not as bcrypt)
std::string User::hashPassword(const std::string& password) {
    // convert each character and combine
    unsigned long hash = 5381;
    
    for (char c : password) {
        hash = ((hash << 5) + hash) + c;  // hash*33 +c
    }
    // Convert to hex string
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

bool User::verifyPassword(const std::string& password, const std::string& hash) {
    // Hash the input password and compare
    return hashPassword(password) == hash;
}

//User Class
User::User() : id(0) {}

User::User(const std::string& uname, const std::string& pass, const std::string& name): id(0), username(uname), fullName(name) {
    // Store hashed password but not a splain text
    password = hashPassword(pass);
}

void User::addAccount(std::shared_ptr<Account> account) {
    accounts.push_back(account);
}

std::shared_ptr<Account> User::getAccountByNumber(const std::string& accNum) {
    // Search through all accounts
    for (auto& acc : accounts) {
        if (acc->getAccountNumber() == accNum) {
            return acc;
        }
    }
    // Return nullptr if not found
    return nullptr;
}

double User::getTotalBalance() const {
    double total = 0.0;
    for (const auto& acc : accounts) {
        total += acc->getBalance();
    }
    return total;
}
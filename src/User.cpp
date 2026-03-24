#include "User.hpp"
#include <sstream>
#include <iomanip>

User::User() : id(0) {}

User::User(const std::string& uname, const std::string& pass, const std::string& name)
    : id(0), username(uname), fullName(name) {
    password = hashPassword(pass);
}

// Destructor - manually delete every Account we own
User::~User() {
    for (int i = 0; i < (int)accounts.size(); i++) {
        delete accounts[i];
    }
    accounts.clear();
}

int User::getId() const { return id; }
std::string User::getUsername() const { return username; }
std::string User::getPassword() const { return password; }
std::string User::getFullName() const { return fullName; }
std::vector<Account*>& User::getAccounts() { return accounts; }

void User::setId(int newId) { id = newId; }
void User::setUsername(const std::string& name) { username = name; }
void User::setPassword(const std::string& pass) { password = pass; }
void User::setFullName(const std::string& name) { fullName = name; }

void User::addAccount(Account* account) {
    accounts.push_back(account);
}

Account* User::getAccountByNumber(const std::string& accNum) {
    for (int i = 0; i < (int)accounts.size(); i++) {
        if (accounts[i]->getAccountNumber() == accNum) {
            return accounts[i];
        }
    }
    return nullptr;
}

double User::getTotalBalance() const {
    double total = 0.0;
    for (int i = 0; i < (int)accounts.size(); i++) {
        total += accounts[i]->getBalance();
    }
    return total;
}

// Delete all accounts and clear the vector
void User::clearAccounts() {
    for (int i = 0; i < (int)accounts.size(); i++) {
        delete accounts[i];
    }
    accounts.clear();
}

// Simple djb2 hash - not secure, just for demo
std::string User::hashPassword(const std::string& password) {
    unsigned long hash = 5381;
    for (char c : password) {
        hash = ((hash << 5) + hash) + c;
    }
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

bool User::verifyPassword(const std::string& password, const std::string& hash) {
    return hashPassword(password) == hash;
}
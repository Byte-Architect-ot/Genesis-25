#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <memory>
#include "Account.hpp"

class User {
private:
    int id;
    std::string username;
    std::string password;  // This will be hashed
    std::string fullName;
    
    // Vector of smart pointers to Account objects
    // shared_ptr automatically manages memory
    std::vector<std::shared_ptr<Account>> accounts;

public:
    // Constructor
    User();
    User(const std::string& username, const std::string& password, const std::string& name);

    // Getters
    int getId() const { return id; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
    std::string getFullName() const { return fullName; }
    std::vector<std::shared_ptr<Account>>& getAccounts() { return accounts; }

    // Setters
    void setId(int newId) { id = newId; }
    void setUsername(const std::string& name) { username = name; }
    void setPassword(const std::string& pass) { password = pass; }
    void setFullName(const std::string& name) { fullName = name; }

    // Account management
    void addAccount(std::shared_ptr<Account> account);
    std::shared_ptr<Account> getAccountByNumber(const std::string& accNum);
    double getTotalBalance() const;

    // Password hashing (simple version)
    static std::string hashPassword(const std::string& password);
    static bool verifyPassword(const std::string& password, const std::string& hash);
};

#endif
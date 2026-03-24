#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include "Account.hpp"

class User {
private:
    int id;
    std::string username;
    std::string password;
    std::string fullName;

    // Raw pointers - we manually delete these in the destructor
    std::vector<Account*> accounts;

public:
    User();
    User(const std::string& username, const std::string& password, const std::string& name);
    ~User(); // Destructor deletes all Account* pointers

    // Getters
    int getId() const;
    std::string getUsername() const;
    std::string getPassword() const;
    std::string getFullName() const;
    std::vector<Account*>& getAccounts();

    // Setters
    void setId(int newId);
    void setUsername(const std::string& name);
    void setPassword(const std::string& pass);
    void setFullName(const std::string& name);

    // Account management
    void addAccount(Account* account);
    Account* getAccountByNumber(const std::string& accNum);
    double getTotalBalance() const;
    void clearAccounts(); // Deletes all accounts and empties the vector

    // Simple password hashing (djb2 algorithm)
    static std::string hashPassword(const std::string& password);
    static bool verifyPassword(const std::string& password, const std::string& hash);
};

#endif
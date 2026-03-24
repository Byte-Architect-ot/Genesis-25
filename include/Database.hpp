#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>
#include <sqlite3.h>
#include "User.hpp"
#include "Account.hpp"

class Database {
private:
    sqlite3* db;
    std::string dbPath;

    bool executeSQL(const std::string& sql);
    void createTables();

public:
    Database(const std::string& path = "data/bank.db");
    ~Database();

    // User operations
    bool addUser(User& user);
    User* getUser(const std::string& username); // Caller must delete
    bool userExists(const std::string& username);

    // Account operations - returns new'd pointers, caller must delete
    bool addAccount(Account& account);
    std::vector<Account*> getUserAccounts(int userId);
    bool updateBalance(int accountId, double newBalance);

    bool isConnected() const { return db != nullptr; }
};

#endif
#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>
#include <memory>
#include <sqlite3.h>
#include "User.hpp"
#include "Account.hpp"

class Database {
private:
    sqlite3* db;           // SQLite database connection
    std::string dbPath;    // Path to database file
    
    // Private helper function to run SQL
    bool executeSQL(const std::string& sql);
    
    // Create tables if they don't exist
    void createTables();

public:
    // Constructor - opens db
    Database(const std::string& path = "data/bank.db");
    
    // Destructor - closes db
    ~Database();

    // User ops
    bool addUser(User& user);
    std::unique_ptr<User> getUser(const std::string& username);
    bool userExists(const std::string& username);

    // Account ops
    bool addAccount(Account& account);
    std::vector<std::shared_ptr<Account>> getUserAccounts(int userId);
    bool updateBalance(int accountId, double newBalance);
    std::shared_ptr<Account> getAccountByNumber(const std::string& accNumber);

    // Transaction ops
    bool addTransaction(int accountId, const std::string& type, 
                       double amount, double balanceAfter, 
                       const std::string& description);
    std::vector<Transaction> getTransactions(int accountId);

    // Check if db is connected
    bool isConnected() const { return db != nullptr; }
};

#endif
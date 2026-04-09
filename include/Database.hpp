#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "User.hpp"
#include "Account.hpp"
#include "Transaction.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>

class Database {
private:
    sqlite3* db;
    std::string dbPath;

private:
    bool exec(const std::string& sql);
    bool prepare(const std::string& sql, sqlite3_stmt** stmt) const;

    bool ensureTables();
    bool ensureAdminUser();

    static std::string nowDateTime();
    static std::string sha256Hex(const std::string& s);

    int getNextAccountId() const;
    std::string makeAccountNumberFromId(int id) const;

    bool insertTransaction(const std::string& type,
                           int fromAccountId,
                           int toAccountId,
                           double amount,
                           const std::string& timestamp);

public:
    Database(const std::string& path = "data/bank.db");
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool registerUser(const std::string& username,
                      const std::string& plainPassword,
                      const std::string& fullName,
                      std::string& err);

    bool authenticate(const std::string& username,
                      const std::string& plainPassword,
                      User& outUser,
                      std::string& err) const;

    bool createAccount(int userId,
                       const std::string& accountType,
                       std::string& outAccountNumber,
                       std::string& err);

    std::vector<std::unique_ptr<Account>> getAccountsForUser(int userId) const;
    bool getAccountByNumber(const std::string& accountNumber, std::unique_ptr<Account>& out) const;
    bool getAccountById(int accountId, std::unique_ptr<Account>& out) const;

    bool deposit(int accountId, double amount, std::string& err);
    bool withdraw(int accountId, double amount, std::string& err);
    bool transfer(int fromAccountId, const std::string& toAccountNumber, double amount, std::string& err);

    std::vector<Transaction> getTransactionsForAccount(int accountId) const;

    std::vector<User> getAllUsers() const;
    std::vector<std::unique_ptr<Account>> getAllAccounts() const;
};

#endif
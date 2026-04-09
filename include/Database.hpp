#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "User.hpp"
#include "Account.hpp"
#include "Transaction.hpp"
#include "Loan.hpp"
#include <string>
#include <vector>

class Database {
private:
    std::string usersFile;
    std::string accountsFile;
    std::string transactionsFile;
    std::string loansFile;

private:
    static std::string trim(const std::string& s);
    static std::vector<std::string> split(const std::string& s, char delim);

    int getNextIdFromFile(const std::string& path) const;

    std::vector<std::string> readAllLines(const std::string& path) const;
    bool writeAllLines(const std::string& path, const std::vector<std::string>& lines) const;
    bool appendLine(const std::string& path, const std::string& line) const;

public:
    Database(const std::string& usersFile = "data/users.txt",
             const std::string& accountsFile = "data/accounts.txt",
             const std::string& transactionsFile = "data/transactions.txt",
             const std::string& loansFile = "data/loans.txt");

    bool addUser(User& user);
    User* getUser(const std::string& username) const;
    std::vector<User*> getAllUsers() const;

    bool addAccount(Account& account);
    std::vector<Account*> getUserAccounts(const User& user) const;
    bool updateBalance(int accountId, double newBalance);

    bool addTransaction(Transaction& t);
    std::vector<Transaction*> getAccountTransactions(int accountId) const;

    bool addLoan(Loan& l);
    std::vector<Loan*> getUserLoans(int userId) const;
    std::vector<Loan*> getAllLoans() const;
    bool updateLoanStatus(int loanId, const std::string& status);
};

#endif
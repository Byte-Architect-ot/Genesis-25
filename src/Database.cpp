#include "Database.hpp"
#include <iostream>
#include <sstream>

//Database Class
Database::Database(const std::string& path) : db(nullptr), dbPath(path) {
    // Open database connection
    int result = sqlite3_open(path.c_str(), &db);
    if (result != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
    } else {
        std::cout << "Database connected: " << path << std::endl;
        createTables();
    }
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
        std::cout << "Database connection closed" << std::endl;
    }
}

bool Database::executeSQL(const std::string& sql) {
    char* errorMsg = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
    if (result != SQLITE_OK) {
        std::cerr << "SQL Error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        return false;
    }
    return true;
}

void Database::createTables() {
    // Create users table
    std::string createUsers = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            full_name TEXT NOT NULL
        );
    )";
    
    // Create accounts table
    std::string createAccounts = R"(
        CREATE TABLE IF NOT EXISTS accounts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            account_number TEXT UNIQUE NOT NULL,
            account_type TEXT NOT NULL,
            balance REAL DEFAULT 0.0,
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
    )";
    
    // Create transactions table
    std::string createTransactions = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            account_id INTEGER NOT NULL,
            type TEXT NOT NULL,
            amount REAL NOT NULL,
            balance_after REAL NOT NULL,
            description TEXT,
            date TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (account_id) REFERENCES accounts(id)
        );
    )";
    
    executeSQL(createUsers);
    executeSQL(createAccounts);
    executeSQL(createTransactions);
    
    std::cout << "Database tables created" << std::endl;
}

//Things that user can do 
bool Database::addUser(User& user) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (username, password, full_name) VALUES (?, ?, ?);";
    
    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement" << std::endl;
        return false;
    }
    
    // Bind values to placeholders (? marks)
    sqlite3_bind_text(stmt, 1, user.getUsername().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.getPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.getFullName().c_str(), -1, SQLITE_TRANSIENT);
    
    // Execute the statement
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result == SQLITE_DONE) {
        // Get the ID of the newly inserted user
        user.setId(static_cast<int>(sqlite3_last_insert_rowid(db)));
        std::cout << "User created with ID: " << user.getId() << std::endl;
        return true;
    }
    
    std::cerr << "Failed to create user" << std::endl;
    return false;
}

std::unique_ptr<User> Database::getUser(const std::string& username) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, username, password, full_name FROM users WHERE username = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Create new User object
        auto user = std::make_unique<User>();
        
        // Read data from database
        user->setId(sqlite3_column_int(stmt, 0));
        user->setUsername(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        user->setPassword(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        user->setFullName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        
        sqlite3_finalize(stmt);
        return user;
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

bool Database::userExists(const std::string& username) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT COUNT(*) FROM users WHERE username = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    
    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }
    
    sqlite3_finalize(stmt);
    return exists;
}

//Account can have these ops!
bool Database::addAccount(Account& account) {
    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO accounts (user_id, account_number, account_type, balance) 
        VALUES (?, ?, ?, ?);
    )";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, account.getUserId());
    sqlite3_bind_text(stmt, 2, account.getAccountNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, account.getAccountType().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, account.getBalance());
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result == SQLITE_DONE) {
        account.setId(static_cast<int>(sqlite3_last_insert_rowid(db)));
        std::cout << "Account created: " << account.getAccountNumber() << std::endl;
        return true;
    }
    
    return false;
}

std::vector<std::shared_ptr<Account>> Database::getUserAccounts(int userId) {
    std::vector<std::shared_ptr<Account>> accounts;
    
    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT id, account_number, account_type, balance 
        FROM accounts WHERE user_id = ?;
    )";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return accounts;
    }
    
    sqlite3_bind_int(stmt, 1, userId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        std::shared_ptr<Account> account;
        
        // Create appropriate account type
        if (type == "SAVINGS") {
            account = std::make_shared<SavingsAccount>(userId);
        } else {
            account = std::make_shared<CurrentAccount>(userId);
        }
        
        // Set data from database
        account->setId(sqlite3_column_int(stmt, 0));
        account->setAccountNumber(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        account->setBalance(sqlite3_column_double(stmt, 3));
        
        accounts.push_back(account);
    }
    
    sqlite3_finalize(stmt);
    return accounts;
}

bool Database::updateBalance(int accountId, double newBalance) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE accounts SET balance = ? WHERE id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_int(stmt, 2, accountId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

std::shared_ptr<Account> Database::getAccountByNumber(const std::string& accNumber) {
    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT id, user_id, account_number, account_type, balance 
        FROM accounts WHERE account_number = ?;
    )";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }
    
    sqlite3_bind_text(stmt, 1, accNumber.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int userId = sqlite3_column_int(stmt, 1);
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        std::shared_ptr<Account> account;
        
        if (type == "SAVINGS") {
            account = std::make_shared<SavingsAccount>(userId);
        } else {
            account = std::make_shared<CurrentAccount>(userId);
        }
        
        account->setId(sqlite3_column_int(stmt, 0));
        account->setAccountNumber(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        account->setBalance(sqlite3_column_double(stmt, 4));
        
        sqlite3_finalize(stmt);
        return account;
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

//Transaction implementation
bool Database::addTransaction(int accountId, const std::string& type,
                              double amount, double balanceAfter,
                              const std::string& description) {
    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO transactions (account_id, type, amount, balance_after, description)
        VALUES (?, ?, ?, ?, ?);
    )";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_text(stmt, 2, type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, amount);
    sqlite3_bind_double(stmt, 4, balanceAfter);
    sqlite3_bind_text(stmt, 5, description.c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

std::vector<Transaction> Database::getTransactions(int accountId) {
    std::vector<Transaction> transactions;
    
    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT id, type, amount, balance_after, description, date 
        FROM transactions WHERE account_id = ? 
        ORDER BY date DESC LIMIT 50;
    )";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return transactions;
    }
    
    sqlite3_bind_int(stmt, 1, accountId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transaction txn;
        txn.id = sqlite3_column_int(stmt, 0);
        txn.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        txn.amount = sqlite3_column_double(stmt, 2);
        txn.balanceAfter = sqlite3_column_double(stmt, 3);
        
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        txn.description = desc ? desc : "";
        
        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        txn.date = date ? date : "";
        
        transactions.push_back(txn);
    }
    
    sqlite3_finalize(stmt);
    return transactions;
}
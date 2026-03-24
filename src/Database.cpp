#include "Database.hpp"
#include <iostream>

Database::Database(const std::string& path) : db(nullptr), dbPath(path) {
    int result = sqlite3_open(path.c_str(), &db);
    if (result != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
    } else {
        createTables();
    }
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
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
    // Just two simple tables: users and accounts
    executeSQL(
        "CREATE TABLE IF NOT EXISTS users ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   username TEXT UNIQUE NOT NULL,"
        "   password TEXT NOT NULL,"
        "   full_name TEXT NOT NULL"
        ");"
    );

    executeSQL(
        "CREATE TABLE IF NOT EXISTS accounts ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   user_id INTEGER NOT NULL,"
        "   account_number TEXT UNIQUE NOT NULL,"
        "   account_type TEXT NOT NULL,"
        "   balance REAL DEFAULT 0.0,"
        "   FOREIGN KEY (user_id) REFERENCES users(id)"
        ");"
    );
}

bool Database::addUser(User& user) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (username, password, full_name) VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, user.getUsername().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.getPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.getFullName().c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result == SQLITE_DONE) {
        user.setId(static_cast<int>(sqlite3_last_insert_rowid(db)));
        return true;
    }
    return false;
}

// Returns a new User* - caller is responsible for deleting it
User* Database::getUser(const std::string& username) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, username, password, full_name FROM users WHERE username = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return nullptr;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User* user = new User();
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

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return exists;
}

bool Database::addAccount(Account& account) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO accounts (user_id, account_number, account_type, balance) "
                      "VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, account.getUserId());
    sqlite3_bind_text(stmt, 2, account.getAccountNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, account.getAccountType().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, account.getBalance());

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result == SQLITE_DONE) {
        account.setId(static_cast<int>(sqlite3_last_insert_rowid(db)));
        return true;
    }
    return false;
}

// Returns new'd Account* pointers - caller must delete them
std::vector<Account*> Database::getUserAccounts(int userId) {
    std::vector<Account*> accounts;

    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, account_number, account_type, balance "
                      "FROM accounts WHERE user_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return accounts;

    sqlite3_bind_int(stmt, 1, userId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        Account* account = nullptr;
        if (type == "SAVINGS") {
            account = new SavingsAccount(userId);
        } else {
            account = new CurrentAccount(userId);
        }

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

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_int(stmt, 2, accountId);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return result == SQLITE_DONE;
}
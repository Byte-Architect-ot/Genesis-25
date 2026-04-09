#include "Database.hpp"
#include "../third_party/picosha2.h"
#include <sstream>
#include <iomanip>
#include <ctime>

Database::Database(const std::string& path) : db(nullptr), dbPath(path) {
    sqlite3_open(dbPath.c_str(), &db);
    ensureTables();
    ensureAdminUser();
}

Database::~Database() {
    if (db) sqlite3_close(db);
}

bool Database::exec(const std::string& sql) {
    return sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
}

bool Database::prepare(const std::string& sql, sqlite3_stmt** stmt) const {
    return sqlite3_prepare_v2(db, sql.c_str(), -1, stmt, nullptr) == SQLITE_OK;
}

std::string Database::nowDateTime() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

std::string Database::sha256Hex(const std::string& s) {
    return picosha2::hash256_hex_string(s);
}

bool Database::ensureTables() {
    bool ok = true;

    ok = ok && exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password_hash TEXT NOT NULL, "
        "full_name TEXT NOT NULL, "
        "role TEXT NOT NULL"
        ");"
    );

    ok = ok && exec(
        "CREATE TABLE IF NOT EXISTS accounts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "account_number TEXT UNIQUE NOT NULL, "
        "account_type TEXT NOT NULL, "
        "balance REAL NOT NULL DEFAULT 0.0, "
        "FOREIGN KEY(user_id) REFERENCES users(id)"
        ");"
    );

    ok = ok && exec(
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "type TEXT NOT NULL, "
        "from_account_id INTEGER, "
        "to_account_id INTEGER, "
        "amount REAL NOT NULL, "
        "timestamp TEXT NOT NULL"
        ");"
    );

    return ok;
}

bool Database::ensureAdminUser() {
    sqlite3_stmt* stmt = nullptr;
    if (!prepare("SELECT id FROM users WHERE username = ?;", &stmt)) return false;

    sqlite3_bind_text(stmt, 1, "admin", -1, SQLITE_TRANSIENT);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);

    if (exists) return true;

    std::string err;
    return registerUser("admin", "admin", "Administrator", err) && exec("UPDATE users SET role='ADMIN' WHERE username='admin';");
}

bool Database::registerUser(const std::string& username,
                            const std::string& plainPassword,
                            const std::string& fullName,
                            std::string& err) {
    err.clear();
    if (username.empty() || plainPassword.empty() || fullName.empty()) {
        err = "Empty fields not allowed";
        return false;
    }

    sqlite3_stmt* stmt = nullptr;
    if (!prepare("INSERT INTO users(username, password_hash, full_name, role) VALUES(?, ?, ?, ?);", &stmt)) {
        err = "Database error";
        return false;
    }

    std::string hash = sha256Hex(plainPassword);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, fullName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, "USER", -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        err = "Username already exists";
        return false;
    }

    return true;
}

bool Database::authenticate(const std::string& username,
                            const std::string& plainPassword,
                            User& outUser,
                            std::string& err) const {
    err.clear();

    sqlite3_stmt* stmt = nullptr;
    if (!prepare("SELECT id, username, password_hash, full_name, role FROM users WHERE username = ?;", &stmt)) {
        err = "Database error";
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        err = "Invalid login";
        return false;
    }

    int id = sqlite3_column_int(stmt, 0);
    std::string uname = (const char*)sqlite3_column_text(stmt, 1);
    std::string hash = (const char*)sqlite3_column_text(stmt, 2);
    std::string fname = (const char*)sqlite3_column_text(stmt, 3);
    std::string role = (const char*)sqlite3_column_text(stmt, 4);

    sqlite3_finalize(stmt);

    std::string inputHash = sha256Hex(plainPassword);
    if (inputHash != hash) {
        err = "Invalid login";
        return false;
    }

    outUser = User(id, uname, hash, fname, role);
    return true;
}

int Database::getNextAccountId() const {
    sqlite3_stmt* stmt = nullptr;
    if (!prepare("SELECT IFNULL(MAX(id), 0) + 1 FROM accounts;", &stmt)) return 1;
    int nextId = 1;
    if (sqlite3_step(stmt) == SQLITE_ROW) nextId = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    if (nextId <= 0) nextId = 1;
    return nextId;
}

std::string Database::makeAccountNumberFromId(int id) const {
    std::ostringstream out;
    out << "AC" << std::setw(6) << std::setfill('0') << id;
    return out.str();
}

bool Database::createAccount(int userId,
                             const std::string& accountType,
                             std::string& outAccountNumber,
                             std::string& err) {
    err.clear();
    outAccountNumber.clear();

    std::string t = accountType;
    if (t != "SAVINGS" && t != "CURRENT") {
        err = "Invalid account type";
        return false;
    }

    int nextId = getNextAccountId();
    std::string accNo = makeAccountNumberFromId(nextId);

    sqlite3_stmt* stmt = nullptr;
    if (!prepare("INSERT INTO accounts(user_id, account_number, account_type, balance) VALUES(?, ?, ?, ?);", &stmt)) {
        err = "Database error";
        return false;
    }

    double opening = 500.0;

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, accNo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, t.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, opening);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        err = "Account creation failed";
        return false;
    }

    outAccountNumber = accNo;
    return true;
}

static std::unique_ptr<Account> makeAccountObject(const std::string& type,
                                                  int userId,
                                                  const std::string& username,
                                                  const std::string& fullName) {
    if (type == "SAVINGS") return std::make_unique<SavingsAccount>(userId, username, fullName);
    return std::make_unique<CurrentAccount>(userId, username, fullName);
}

std::vector<std::unique_ptr<Account>> Database::getAccountsForUser(int userId) const {
    std::vector<std::unique_ptr<Account>> out;

    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT a.id, a.user_id, u.username, u.full_name, a.account_number, a.account_type, a.balance "
        "FROM accounts a JOIN users u ON a.user_id = u.id "
        "WHERE a.user_id = ? "
        "ORDER BY a.id;";

    if (!prepare(sql, &stmt)) return out;

    sqlite3_bind_int(stmt, 1, userId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int accId = sqlite3_column_int(stmt, 0);
        int uId = sqlite3_column_int(stmt, 1);
        std::string uname = (const char*)sqlite3_column_text(stmt, 2);
        std::string fname = (const char*)sqlite3_column_text(stmt, 3);
        std::string accNo = (const char*)sqlite3_column_text(stmt, 4);
        std::string type = (const char*)sqlite3_column_text(stmt, 5);
        double bal = sqlite3_column_double(stmt, 6);

        auto a = makeAccountObject(type, uId, uname, fname);
        a->setAccountId(accId);
        a->setAccountNumber(accNo);
        a->setBalance(bal);
        out.push_back(std::move(a));
    }

    sqlite3_finalize(stmt);
    return out;
}

bool Database::getAccountByNumber(const std::string& accountNumber, std::unique_ptr<Account>& out) const {
    out.reset();

    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT a.id, a.user_id, u.username, u.full_name, a.account_number, a.account_type, a.balance "
        "FROM accounts a JOIN users u ON a.user_id = u.id "
        "WHERE a.account_number = ?;";

    if (!prepare(sql, &stmt)) return false;

    sqlite3_bind_text(stmt, 1, accountNumber.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return false;
    }

    int accId = sqlite3_column_int(stmt, 0);
    int uId = sqlite3_column_int(stmt, 1);
    std::string uname = (const char*)sqlite3_column_text(stmt, 2);
    std::string fname = (const char*)sqlite3_column_text(stmt, 3);
    std::string accNo = (const char*)sqlite3_column_text(stmt, 4);
    std::string type = (const char*)sqlite3_column_text(stmt, 5);
    double bal = sqlite3_column_double(stmt, 6);

    sqlite3_finalize(stmt);

    auto a = makeAccountObject(type, uId, uname, fname);
    a->setAccountId(accId);
    a->setAccountNumber(accNo);
    a->setBalance(bal);
    out = std::move(a);
    return true;
}

bool Database::getAccountById(int accountId, std::unique_ptr<Account>& out) const {
    out.reset();

    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT a.id, a.user_id, u.username, u.full_name, a.account_number, a.account_type, a.balance "
        "FROM accounts a JOIN users u ON a.user_id = u.id "
        "WHERE a.id = ?;";

    if (!prepare(sql, &stmt)) return false;

    sqlite3_bind_int(stmt, 1, accountId);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return false;
    }

    int accId = sqlite3_column_int(stmt, 0);
    int uId = sqlite3_column_int(stmt, 1);
    std::string uname = (const char*)sqlite3_column_text(stmt, 2);
    std::string fname = (const char*)sqlite3_column_text(stmt, 3);
    std::string accNo = (const char*)sqlite3_column_text(stmt, 4);
    std::string type = (const char*)sqlite3_column_text(stmt, 5);
    double bal = sqlite3_column_double(stmt, 6);

    sqlite3_finalize(stmt);

    auto a = makeAccountObject(type, uId, uname, fname);
    a->setAccountId(accId);
    a->setAccountNumber(accNo);
    a->setBalance(bal);
    out = std::move(a);
    return true;
}

bool Database::insertTransaction(const std::string& type,
                                 int fromAccountId,
                                 int toAccountId,
                                 double amount,
                                 const std::string& timestamp) {
    sqlite3_stmt* stmt = nullptr;
    if (!prepare("INSERT INTO transactions(type, from_account_id, to_account_id, amount, timestamp) VALUES(?, ?, ?, ?, ?);", &stmt)) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_TRANSIENT);

    if (fromAccountId == 0) sqlite3_bind_null(stmt, 2);
    else sqlite3_bind_int(stmt, 2, fromAccountId);

    if (toAccountId == 0) sqlite3_bind_null(stmt, 3);
    else sqlite3_bind_int(stmt, 3, toAccountId);

    sqlite3_bind_double(stmt, 4, amount);
    sqlite3_bind_text(stmt, 5, timestamp.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::deposit(int accountId, double amount, std::string& err) {
    err.clear();
    if (amount <= 0) { err = "Invalid amount"; return false; }

    if (!exec("BEGIN TRANSACTION;")) { err = "Database error"; return false; }

    std::unique_ptr<Account> acc;
    if (!getAccountById(accountId, acc)) {
        exec("ROLLBACK;");
        err = "Account not found";
        return false;
    }

    double newBal = acc->getBalance() + amount;

    sqlite3_stmt* stmt = nullptr;
    if (!prepare("UPDATE accounts SET balance = ? WHERE id = ?;", &stmt)) {
        exec("ROLLBACK;");
        err = "Database error";
        return false;
    }

    sqlite3_bind_double(stmt, 1, newBal);
    sqlite3_bind_int(stmt, 2, accountId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        exec("ROLLBACK;");
        err = "Update failed";
        return false;
    }

    if (!insertTransaction("DEPOSIT", 0, accountId, amount, nowDateTime())) {
        exec("ROLLBACK;");
        err = "Transaction log failed";
        return false;
    }

    exec("COMMIT;");
    return true;
}

bool Database::withdraw(int accountId, double amount, std::string& err) {
    err.clear();
    if (amount <= 0) { err = "Invalid amount"; return false; }

    if (!exec("BEGIN TRANSACTION;")) { err = "Database error"; return false; }

    std::unique_ptr<Account> acc;
    if (!getAccountById(accountId, acc)) {
        exec("ROLLBACK;");
        err = "Account not found";
        return false;
    }

    if (!acc->canWithdraw(amount)) {
        exec("ROLLBACK;");
        err = "Insufficient funds";
        return false;
    }

    double newBal = acc->getBalance() - amount;

    sqlite3_stmt* stmt = nullptr;
    if (!prepare("UPDATE accounts SET balance = ? WHERE id = ?;", &stmt)) {
        exec("ROLLBACK;");
        err = "Database error";
        return false;
    }

    sqlite3_bind_double(stmt, 1, newBal);
    sqlite3_bind_int(stmt, 2, accountId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        exec("ROLLBACK;");
        err = "Update failed";
        return false;
    }

    if (!insertTransaction("WITHDRAW", accountId, 0, amount, nowDateTime())) {
        exec("ROLLBACK;");
        err = "Transaction log failed";
        return false;
    }

    exec("COMMIT;");
    return true;
}

bool Database::transfer(int fromAccountId, const std::string& toAccountNumber, double amount, std::string& err) {
    err.clear();
    if (amount <= 0) { err = "Invalid amount"; return false; }

    if (!exec("BEGIN TRANSACTION;")) { err = "Database error"; return false; }

    std::unique_ptr<Account> fromAcc;
    if (!getAccountById(fromAccountId, fromAcc)) {
        exec("ROLLBACK;");
        err = "From account not found";
        return false;
    }

    std::unique_ptr<Account> toAcc;
    if (!getAccountByNumber(toAccountNumber, toAcc)) {
        exec("ROLLBACK;");
        err = "To account not found";
        return false;
    }

    if (toAcc->getAccountId() == fromAcc->getAccountId()) {
        exec("ROLLBACK;");
        err = "Same account transfer not allowed";
        return false;
    }

    if (!fromAcc->canWithdraw(amount)) {
        exec("ROLLBACK;");
        err = "Insufficient funds";
        return false;
    }

    double fromNew = fromAcc->getBalance() - amount;
    double toNew = toAcc->getBalance() + amount;

    sqlite3_stmt* u1 = nullptr;
    if (!prepare("UPDATE accounts SET balance = ? WHERE id = ?;", &u1)) {
        exec("ROLLBACK;");
        err = "Database error";
        return false;
    }
    sqlite3_bind_double(u1, 1, fromNew);
    sqlite3_bind_int(u1, 2, fromAcc->getAccountId());
    int rc1 = sqlite3_step(u1);
    sqlite3_finalize(u1);
    if (rc1 != SQLITE_DONE) {
        exec("ROLLBACK;");
        err = "Update failed";
        return false;
    }

    sqlite3_stmt* u2 = nullptr;
    if (!prepare("UPDATE accounts SET balance = ? WHERE id = ?;", &u2)) {
        exec("ROLLBACK;");
        err = "Database error";
        return false;
    }
    sqlite3_bind_double(u2, 1, toNew);
    sqlite3_bind_int(u2, 2, toAcc->getAccountId());
    int rc2 = sqlite3_step(u2);
    sqlite3_finalize(u2);
    if (rc2 != SQLITE_DONE) {
        exec("ROLLBACK;");
        err = "Update failed";
        return false;
    }

    if (!insertTransaction("TRANSFER", fromAcc->getAccountId(), toAcc->getAccountId(), amount, nowDateTime())) {
        exec("ROLLBACK;");
        err = "Transaction log failed";
        return false;
    }

    exec("COMMIT;");
    return true;
}

std::vector<Transaction> Database::getTransactionsForAccount(int accountId) const {
    std::vector<Transaction> out;

    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT id, type, IFNULL(from_account_id, 0), IFNULL(to_account_id, 0), amount, timestamp "
        "FROM transactions "
        "WHERE from_account_id = ? OR to_account_id = ? "
        "ORDER BY id;";

    if (!prepare(sql, &stmt)) return out;

    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_int(stmt, 2, accountId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string type = (const char*)sqlite3_column_text(stmt, 1);
        int fromId = sqlite3_column_int(stmt, 2);
        int toId = sqlite3_column_int(stmt, 3);
        double amt = sqlite3_column_double(stmt, 4);
        std::string ts = (const char*)sqlite3_column_text(stmt, 5);

        out.emplace_back(id, type, fromId, toId, amt, ts);
    }

    sqlite3_finalize(stmt);
    return out;
}

std::vector<User> Database::getAllUsers() const {
    std::vector<User> out;

    sqlite3_stmt* stmt = nullptr;
    if (!prepare("SELECT id, username, password_hash, full_name, role FROM users ORDER BY id;", &stmt)) {
        return out;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string uname = (const char*)sqlite3_column_text(stmt, 1);
        std::string hash = (const char*)sqlite3_column_text(stmt, 2);
        std::string name = (const char*)sqlite3_column_text(stmt, 3);
        std::string role = (const char*)sqlite3_column_text(stmt, 4);
        out.emplace_back(id, uname, hash, name, role);
    }

    sqlite3_finalize(stmt);
    return out;
}

std::vector<std::unique_ptr<Account>> Database::getAllAccounts() const {
    std::vector<std::unique_ptr<Account>> out;

    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT a.id, a.user_id, u.username, u.full_name, a.account_number, a.account_type, a.balance "
        "FROM accounts a JOIN users u ON a.user_id = u.id "
        "ORDER BY a.id;";

    if (!prepare(sql, &stmt)) return out;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int accId = sqlite3_column_int(stmt, 0);
        int uId = sqlite3_column_int(stmt, 1);
        std::string uname = (const char*)sqlite3_column_text(stmt, 2);
        std::string fname = (const char*)sqlite3_column_text(stmt, 3);
        std::string accNo = (const char*)sqlite3_column_text(stmt, 4);
        std::string type = (const char*)sqlite3_column_text(stmt, 5);
        double bal = sqlite3_column_double(stmt, 6);

        auto a = makeAccountObject(type, uId, uname, fname);
        a->setAccountId(accId);
        a->setAccountNumber(accNo);
        a->setBalance(bal);
        out.push_back(std::move(a));
    }

    sqlite3_finalize(stmt);
    return out;
}
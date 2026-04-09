#include "Database.hpp"
#include "../third_party/picosha2.h"
#include <utility>

Database::Database(const std::string& path) : db(nullptr) {
    sqlite3_open(path.c_str(), &db);
}

Database::~Database() {
    if (db) sqlite3_close(db);
}

bool Database::exec(const std::string& sql) const {
    return sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
}

std::string Database::sha256Hex(const std::string& s) {
    return picosha2::hash256_hex_string(s);
}

bool Database::init() {
    if (!db) return false;

    bool ok = exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password_hash TEXT NOT NULL, "
        "full_name TEXT NOT NULL, "
        "role TEXT NOT NULL"
        ");"
    );

    if (!ok) return false;

    std::string err;
    registerUser("admin", "admin", "Administrator", "ADMIN", err);
    return true;
}

bool Database::registerUser(const std::string& username,
                            const std::string& plainPassword,
                            const std::string& fullName,
                            const std::string& role,
                            std::string& err) const {
    err.clear();

    if (username.empty() || plainPassword.empty() || fullName.empty() || role.empty()) {
        err = "Empty fields not allowed";
        return false;
    }

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO users(username, password_hash, full_name, role) VALUES(?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        err = "Database error";
        return false;
    }

    std::string hash = sha256Hex(plainPassword);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, fullName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, role.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        err = "Username already exists";
        return false;
    }

    return true;
}

bool Database::login(const std::string& username,
                     const std::string& plainPassword,
                     AuthUser& outUser,
                     std::string& err) const {
    err.clear();

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, username, password_hash, full_name, role FROM users WHERE username = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
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
    std::string storedHash = (const char*)sqlite3_column_text(stmt, 2);
    std::string fullName = (const char*)sqlite3_column_text(stmt, 3);
    std::string role = (const char*)sqlite3_column_text(stmt, 4);

    sqlite3_finalize(stmt);

    if (sha256Hex(plainPassword) != storedHash) {
        err = "Invalid login";
        return false;
    }

    outUser.id = id;
    outUser.username = std::move(uname);
    outUser.fullName = std::move(fullName);
    outUser.role = std::move(role);
    return true;
}
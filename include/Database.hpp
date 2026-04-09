#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <sqlite3.h>

struct AuthUser {
    int id = 0;
    std::string username;
    std::string fullName;
    std::string role;
};

class Database {
private:
    sqlite3* db;

private:
    bool exec(const std::string& sql) const;
    static std::string sha256Hex(const std::string& s);

public:
    Database(const std::string& path = "data/users.db");
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool init();

    bool registerUser(const std::string& username,
                      const std::string& plainPassword,
                      const std::string& fullName,
                      const std::string& role,
                      std::string& err) const;

    bool login(const std::string& username,
               const std::string& plainPassword,
               AuthUser& outUser,
               std::string& err) const;
};

#endif
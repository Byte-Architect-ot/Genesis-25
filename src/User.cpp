#include "User.hpp"

User::User() : id(0), username(""), passwordHash(""), fullName(""), role("USER") {}

User::User(int id,
           const std::string& username,
           const std::string& passwordHash,
           const std::string& fullName,
           const std::string& role)
    : id(id), username(username), passwordHash(passwordHash), fullName(fullName), role(role) {}

int User::getId() const { return id; }
void User::setId(int v) { id = v; }

const std::string& User::getUsername() const { return username; }
void User::setUsername(const std::string& v) { username = v; }

const std::string& User::getPasswordHash() const { return passwordHash; }
void User::setPasswordHash(const std::string& v) { passwordHash = v; }

const std::string& User::getFullName() const { return fullName; }
void User::setFullName(const std::string& v) { fullName = v; }

const std::string& User::getRole() const { return role; }
void User::setRole(const std::string& v) { role = v; }
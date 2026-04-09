#include "User.hpp"

User::User()
    : id(0), username(""), password(""), fullName(""), role("USER"),
      mobile(""), email(""), address(""), aadhaar("") {}

User::User(const std::string& username,
           const std::string& password,
           const std::string& fullName,
           const std::string& role,
           const std::string& mobile,
           const std::string& email,
           const std::string& address,
           const std::string& aadhaar)
    : id(0), username(username), password(password), fullName(fullName), role(role),
      mobile(mobile), email(email), address(address), aadhaar(aadhaar) {}

int User::getId() const { return id; }
void User::setId(int newId) { id = newId; }

const std::string& User::getUsername() const { return username; }
void User::setUsername(const std::string& v) { username = v; }

const std::string& User::getPassword() const { return password; }
void User::setPassword(const std::string& v) { password = v; }

const std::string& User::getFullName() const { return fullName; }
void User::setFullName(const std::string& v) { fullName = v; }

const std::string& User::getRole() const { return role; }
void User::setRole(const std::string& v) { role = v; }

const std::string& User::getMobile() const { return mobile; }
void User::setMobile(const std::string& v) { mobile = v; }

const std::string& User::getEmail() const { return email; }
void User::setEmail(const std::string& v) { email = v; }

const std::string& User::getAddress() const { return address; }
void User::setAddress(const std::string& v) { address = v; }

const std::string& User::getAadhaar() const { return aadhaar; }
void User::setAadhaar(const std::string& v) { aadhaar = v; }
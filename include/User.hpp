#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
private:
    int id;
    std::string username;
    std::string passwordHash;
    std::string fullName;
    std::string role;

public:
    User();
    User(int id,
         const std::string& username,
         const std::string& passwordHash,
         const std::string& fullName,
         const std::string& role);

    int getId() const;
    void setId(int v);

    const std::string& getUsername() const;
    void setUsername(const std::string& v);

    const std::string& getPasswordHash() const;
    void setPasswordHash(const std::string& v);

    const std::string& getFullName() const;
    void setFullName(const std::string& v);

    const std::string& getRole() const;
    void setRole(const std::string& v);
};

#endif
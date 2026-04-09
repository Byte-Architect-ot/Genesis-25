#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
private:
    int id;
    std::string username;
    std::string password;
    std::string fullName;
    std::string role;
    std::string mobile;
    std::string email;
    std::string address;
    std::string aadhaar;

public:
    User();
    User(const std::string& username,
         const std::string& password,
         const std::string& fullName,
         const std::string& role,
         const std::string& mobile,
         const std::string& email,
         const std::string& address,
         const std::string& aadhaar);

    int getId() const;
    void setId(int newId);

    const std::string& getUsername() const;
    void setUsername(const std::string& v);

    const std::string& getPassword() const;
    void setPassword(const std::string& v);

    const std::string& getFullName() const;
    void setFullName(const std::string& v);

    const std::string& getRole() const;
    void setRole(const std::string& v);

    const std::string& getMobile() const;
    void setMobile(const std::string& v);

    const std::string& getEmail() const;
    void setEmail(const std::string& v);

    const std::string& getAddress() const;
    void setAddress(const std::string& v);

    const std::string& getAadhaar() const;
    void setAadhaar(const std::string& v);
};

#endif
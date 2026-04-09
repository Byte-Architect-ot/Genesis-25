#ifndef LOAN_HPP
#define LOAN_HPP

#include <string>

class Loan {
private:
    int id;
    std::string username;
    double amount;
    double interestRate;
    std::string status;
    std::string timestamp;

public:
    Loan();
    Loan(int id, const std::string& username, double amount, double interestRate, const std::string& status, const std::string& timestamp);

    int getId() const;
    const std::string& getUsername() const;
    double getAmount() const;
    double getInterestRate() const;
    const std::string& getStatus() const;
    void setStatus(const std::string& v);
    const std::string& getTimestamp() const;
};

#endif
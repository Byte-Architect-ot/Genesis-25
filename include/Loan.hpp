#ifndef LOAN_HPP
#define LOAN_HPP

#include <string>

class Loan {
private:
    int id;
    int userId;
    double amount;
    double interestRate;
    std::string status;

public:
    Loan();
    Loan(int userId, double amount, double interestRate, const std::string& status);

    int getId() const;
    void setId(int newId);

    int getUserId() const;
    double getAmount() const;
    double getInterestRate() const;

    const std::string& getStatus() const;
    void setStatus(const std::string& v);
};

#endif
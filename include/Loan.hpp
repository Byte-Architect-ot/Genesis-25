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
    Loan(int id, int userId, double amount, double interestRate, const std::string& status);

    int getId() const;
    void setId(int v);

    int getUserId() const;
    void setUserId(int v);

    double getAmount() const;
    void setAmount(double v);

    double getInterestRate() const;
    void setInterestRate(double v);

    const std::string& getStatus() const;
    void setStatus(const std::string& v);
};

#endif
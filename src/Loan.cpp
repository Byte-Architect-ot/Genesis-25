#include "Loan.hpp"

Loan::Loan() : id(0), userId(0), amount(0.0), interestRate(0.0), status("PENDING") {}

Loan::Loan(int userId, double amount, double interestRate, const std::string& status)
    : id(0), userId(userId), amount(amount), interestRate(interestRate), status(status) {}

int Loan::getId() const { return id; }
void Loan::setId(int newId) { id = newId; }

int Loan::getUserId() const { return userId; }
double Loan::getAmount() const { return amount; }
double Loan::getInterestRate() const { return interestRate; }

const std::string& Loan::getStatus() const { return status; }
void Loan::setStatus(const std::string& v) { status = v; }
#include "Loan.hpp"

Loan::Loan() : id(0), userId(0), amount(0.0), interestRate(0.0), status("PENDING") {}

Loan::Loan(int id, int userId, double amount, double interestRate, const std::string& status)
    : id(id), userId(userId), amount(amount), interestRate(interestRate), status(status) {}

int Loan::getId() const { return id; }
void Loan::setId(int v) { id = v; }

int Loan::getUserId() const { return userId; }
void Loan::setUserId(int v) { userId = v; }

double Loan::getAmount() const { return amount; }
void Loan::setAmount(double v) { amount = v; }

double Loan::getInterestRate() const { return interestRate; }
void Loan::setInterestRate(double v) { interestRate = v; }

const std::string& Loan::getStatus() const { return status; }
void Loan::setStatus(const std::string& v) { status = v; }
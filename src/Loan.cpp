#include "Loan.hpp"

Loan::Loan() : id(0), username(""), amount(0.0), interestRate(0.0), status("PENDING"), timestamp("") {}

Loan::Loan(int id, const std::string& username, double amount, double interestRate, const std::string& status, const std::string& timestamp)
    : id(id), username(username), amount(amount), interestRate(interestRate), status(status), timestamp(timestamp) {}

int Loan::getId() const { return id; }
const std::string& Loan::getUsername() const { return username; }
double Loan::getAmount() const { return amount; }
double Loan::getInterestRate() const { return interestRate; }
const std::string& Loan::getStatus() const { return status; }
void Loan::setStatus(const std::string& v) { status = v; }
const std::string& Loan::getTimestamp() const { return timestamp; }
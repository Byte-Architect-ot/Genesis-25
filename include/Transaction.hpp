#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <string>

class Transaction {
private:
    int id;
    std::string type;
    std::string fromAcc;
    std::string toAcc;
    double amount;
    std::string timestamp;

public:
    Transaction();
    Transaction(int id, const std::string& type, const std::string& fromAcc, const std::string& toAcc, double amount, const std::string& timestamp);

    int getId() const;
    const std::string& getType() const;
    const std::string& getFromAcc() const;
    const std::string& getToAcc() const;
    double getAmount() const;
    const std::string& getTimestamp() const;
};

#endif
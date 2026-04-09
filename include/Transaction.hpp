#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <string>

class Transaction {
private:
    int id;
    int accountId;
    std::string type;
    double amount;
    std::string dateTime;

public:
    Transaction();
    Transaction(int accountId, const std::string& type, double amount, const std::string& dateTime);

    int getId() const;
    void setId(int newId);

    int getAccountId() const;
    const std::string& getType() const;
    double getAmount() const;
    const std::string& getDateTime() const;
};

#endif
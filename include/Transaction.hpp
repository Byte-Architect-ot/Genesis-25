#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <string>

class Transaction {
private:
    int id;
    std::string type;
    int fromAccountId;
    int toAccountId;
    double amount;
    std::string timestamp;

public:
    Transaction();
    Transaction(int id,
                const std::string& type,
                int fromAccountId,
                int toAccountId,
                double amount,
                const std::string& timestamp);

    int getId() const;
    void setId(int v);

    const std::string& getType() const;
    void setType(const std::string& v);

    int getFromAccountId() const;
    void setFromAccountId(int v);

    int getToAccountId() const;
    void setToAccountId(int v);

    double getAmount() const;
    void setAmount(double v);

    const std::string& getTimestamp() const;
    void setTimestamp(const std::string& v);
};

#endif
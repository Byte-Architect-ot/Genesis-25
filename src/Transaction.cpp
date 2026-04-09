#include "Transaction.hpp"

Transaction::Transaction()
    : id(0), type(""), fromAccountId(0), toAccountId(0), amount(0.0), timestamp("") {}

Transaction::Transaction(int id,
                         const std::string& type,
                         int fromAccountId,
                         int toAccountId,
                         double amount,
                         const std::string& timestamp)
    : id(id), type(type), fromAccountId(fromAccountId), toAccountId(toAccountId), amount(amount), timestamp(timestamp) {}

int Transaction::getId() const { return id; }
void Transaction::setId(int v) { id = v; }

const std::string& Transaction::getType() const { return type; }
void Transaction::setType(const std::string& v) { type = v; }

int Transaction::getFromAccountId() const { return fromAccountId; }
void Transaction::setFromAccountId(int v) { fromAccountId = v; }

int Transaction::getToAccountId() const { return toAccountId; }
void Transaction::setToAccountId(int v) { toAccountId = v; }

double Transaction::getAmount() const { return amount; }
void Transaction::setAmount(double v) { amount = v; }

const std::string& Transaction::getTimestamp() const { return timestamp; }
void Transaction::setTimestamp(const std::string& v) { timestamp = v; }
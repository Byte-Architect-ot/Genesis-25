#include "Transaction.hpp"

Transaction::Transaction() : id(0), accountId(0), type(""), amount(0.0), dateTime("") {}

Transaction::Transaction(int accountId, const std::string& type, double amount, const std::string& dateTime)
    : id(0), accountId(accountId), type(type), amount(amount), dateTime(dateTime) {}

int Transaction::getId() const { return id; }
void Transaction::setId(int newId) { id = newId; }

int Transaction::getAccountId() const { return accountId; }
const std::string& Transaction::getType() const { return type; }
double Transaction::getAmount() const { return amount; }
const std::string& Transaction::getDateTime() const { return dateTime; }
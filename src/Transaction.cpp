#include "Transaction.hpp"

Transaction::Transaction() : id(0), type(""), fromAcc(""), toAcc(""), amount(0.0), timestamp("") {}

Transaction::Transaction(int id, const std::string& type, const std::string& fromAcc, const std::string& toAcc, double amount, const std::string& timestamp)
    : id(id), type(type), fromAcc(fromAcc), toAcc(toAcc), amount(amount), timestamp(timestamp) {}

int Transaction::getId() const { return id; }
const std::string& Transaction::getType() const { return type; }
const std::string& Transaction::getFromAcc() const { return fromAcc; }
const std::string& Transaction::getToAcc() const { return toAcc; }
double Transaction::getAmount() const { return amount; }
const std::string& Transaction::getTimestamp() const { return timestamp; }
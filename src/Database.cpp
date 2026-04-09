#include "Database.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

Database::Database(const std::string& usersFile,
                   const std::string& accountsFile,
                   const std::string& transactionsFile,
                   const std::string& loansFile)
    : usersFile(usersFile),
      accountsFile(accountsFile),
      transactionsFile(transactionsFile),
      loansFile(loansFile) {}

std::string Database::trim(const std::string& s) {
    size_t i = 0;
    while (i < s.size() && std::isspace((unsigned char)s[i])) i++;
    size_t j = s.size();
    while (j > i && std::isspace((unsigned char)s[j - 1])) j--;
    return s.substr(i, j - i);
}

std::vector<std::string> Database::split(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) parts.push_back(item);
    return parts;
}

std::vector<std::string> Database::readAllLines(const std::string& path) const {
    std::vector<std::string> lines;
    std::ifstream fin(path);
    if (!fin.is_open()) return lines;

    std::string line;
    while (std::getline(fin, line)) {
        line = trim(line);
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

bool Database::writeAllLines(const std::string& path, const std::vector<std::string>& lines) const {
    std::ofstream fout(path, std::ios::trunc);
    if (!fout.is_open()) return false;
    for (const auto& l : lines) fout << l << "\n";
    return true;
}

bool Database::appendLine(const std::string& path, const std::string& line) const {
    std::ofstream fout(path, std::ios::app);
    if (!fout.is_open()) return false;
    fout << line << "\n";
    return true;
}

int Database::getNextIdFromFile(const std::string& path) const {
    auto lines = readAllLines(path);
    int maxId = 0;
    for (const auto& l : lines) {
        auto parts = split(l, '|');
        if (parts.empty()) continue;
        try {
            int id = std::stoi(parts[0]);
            if (id > maxId) maxId = id;
        } catch (...) {
        }
    }
    return maxId + 1;
}

bool Database::addUser(User& user) {
    if (getUser(user.getUsername()) != nullptr) {
        return false;
    }

    int newId = getNextIdFromFile(usersFile);
    user.setId(newId);

    std::ostringstream out;
    out << user.getId() << "|"
        << user.getUsername() << "|"
        << user.getPassword() << "|"
        << user.getFullName() << "|"
        << user.getRole() << "|"
        << user.getMobile() << "|"
        << user.getEmail() << "|"
        << user.getAddress() << "|"
        << user.getAadhaar();

    return appendLine(usersFile, out.str());
}

User* Database::getUser(const std::string& username) const {
    auto lines = readAllLines(usersFile);
    for (const auto& l : lines) {
        auto p = split(l, '|');
        if (p.size() < 9) continue;
        if (p[1] != username) continue;

        User* u = new User(p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
        try { u->setId(std::stoi(p[0])); } catch (...) { u->setId(0); }
        return u;
    }
    return nullptr;
}

std::vector<User*> Database::getAllUsers() const {
    std::vector<User*> out;
    auto lines = readAllLines(usersFile);
    for (const auto& l : lines) {
        auto p = split(l, '|');
        if (p.size() < 9) continue;
        User* u = new User(p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
        try { u->setId(std::stoi(p[0])); } catch (...) { u->setId(0); }
        out.push_back(u);
    }
    return out;
}

bool Database::addAccount(Account& account) {
    int newId = getNextIdFromFile(accountsFile);
    account.setId(newId);

    std::ostringstream accNo;
    accNo << "AC" << std::setw(6) << std::setfill('0') << newId;
    account.setAccountNumber(accNo.str());

    std::ostringstream out;
    out << account.getId() << "|"
        << account.getUserId() << "|"
        << account.getAccountNumber() << "|"
        << account.getAccountType() << "|"
        << std::fixed << std::setprecision(2) << account.getBalance();

    return appendLine(accountsFile, out.str());
}

std::vector<Account*> Database::getUserAccounts(const User& user) const {
    std::vector<Account*> out;
    auto lines = readAllLines(accountsFile);

    for (const auto& l : lines) {
        auto p = split(l, '|');
        if (p.size() < 5) continue;

        int accId = 0;
        int uId = 0;
        double bal = 0.0;

        try { accId = std::stoi(p[0]); } catch (...) { accId = 0; }
        try { uId = std::stoi(p[1]); } catch (...) { uId = 0; }
        try { bal = std::stod(p[4]); } catch (...) { bal = 0.0; }

        if (uId != user.getId()) continue;

        const std::string& accNo = p[2];
        const std::string& type = p[3];

        Account* a = nullptr;
        if (type == "SAVINGS") a = new SavingsAccount(user.getId(), user.getUsername(), user.getFullName());
        else if (type == "FD") a = new FixedDepositAccount(user.getId(), user.getUsername(), user.getFullName());
        else a = new CurrentAccount(user.getId(), user.getUsername(), user.getFullName());

        a->setId(accId);
        a->setAccountNumber(accNo);
        a->setBalance(bal);

        out.push_back(a);
    }

    return out;
}

bool Database::updateBalance(int accountId, double newBalance) {
    auto lines = readAllLines(accountsFile);
    bool updated = false;

    for (auto& l : lines) {
        auto p = split(l, '|');
        if (p.size() < 5) continue;

        int accId = 0;
        try { accId = std::stoi(p[0]); } catch (...) { accId = 0; }

        if (accId == accountId) {
            std::ostringstream out;
            out << p[0] << "|" << p[1] << "|" << p[2] << "|" << p[3] << "|"
                << std::fixed << std::setprecision(2) << newBalance;
            l = out.str();
            updated = true;
            break;
        }
    }

    if (!updated) return false;
    return writeAllLines(accountsFile, lines);
}

bool Database::addTransaction(Transaction& t) {
    int newId = getNextIdFromFile(transactionsFile);
    t.setId(newId);

    std::ostringstream out;
    out << t.getId() << "|"
        << t.getAccountId() << "|"
        << t.getType() << "|"
        << std::fixed << std::setprecision(2) << t.getAmount() << "|"
        << t.getDateTime();

    return appendLine(transactionsFile, out.str());
}

std::vector<Transaction*> Database::getAccountTransactions(int accountId) const {
    std::vector<Transaction*> out;
    auto lines = readAllLines(transactionsFile);

    for (const auto& l : lines) {
        auto p = split(l, '|');
        if (p.size() < 5) continue;

        int txId = 0;
        int accId = 0;
        double amt = 0.0;

        try { txId = std::stoi(p[0]); } catch (...) { txId = 0; }
        try { accId = std::stoi(p[1]); } catch (...) { accId = 0; }
        try { amt = std::stod(p[3]); } catch (...) { amt = 0.0; }

        if (accId != accountId) continue;

        Transaction* t = new Transaction(accId, p[2], amt, p[4]);
        t->setId(txId);
        out.push_back(t);
    }

    return out;
}

bool Database::addLoan(Loan& l) {
    int newId = getNextIdFromFile(loansFile);
    l.setId(newId);

    std::ostringstream out;
    out << l.getId() << "|"
        << l.getUserId() << "|"
        << std::fixed << std::setprecision(2) << l.getAmount() << "|"
        << std::fixed << std::setprecision(2) << l.getInterestRate() << "|"
        << l.getStatus();

    return appendLine(loansFile, out.str());
}

std::vector<Loan*> Database::getUserLoans(int userId) const {
    std::vector<Loan*> out;
    auto lines = readAllLines(loansFile);

    for (const auto& l : lines) {
        auto p = split(l, '|');
        if (p.size() < 5) continue;

        int loanId = 0;
        int uId = 0;
        double amt = 0.0;
        double rate = 0.0;

        try { loanId = std::stoi(p[0]); } catch (...) { loanId = 0; }
        try { uId = std::stoi(p[1]); } catch (...) { uId = 0; }
        try { amt = std::stod(p[2]); } catch (...) { amt = 0.0; }
        try { rate = std::stod(p[3]); } catch (...) { rate = 0.0; }

        if (uId != userId) continue;

        Loan* loan = new Loan(uId, amt, rate, p[4]);
        loan->setId(loanId);
        out.push_back(loan);
    }

    return out;
}

std::vector<Loan*> Database::getAllLoans() const {
    std::vector<Loan*> out;
    auto lines = readAllLines(loansFile);

    for (const auto& l : lines) {
        auto p = split(l, '|');
        if (p.size() < 5) continue;

        int loanId = 0;
        int uId = 0;
        double amt = 0.0;
        double rate = 0.0;

        try { loanId = std::stoi(p[0]); } catch (...) { loanId = 0; }
        try { uId = std::stoi(p[1]); } catch (...) { uId = 0; }
        try { amt = std::stod(p[2]); } catch (...) { amt = 0.0; }
        try { rate = std::stod(p[3]); } catch (...) { rate = 0.0; }

        Loan* loan = new Loan(uId, amt, rate, p[4]);
        loan->setId(loanId);
        out.push_back(loan);
    }

    return out;
}

bool Database::updateLoanStatus(int loanId, const std::string& status) {
    auto lines = readAllLines(loansFile);
    bool updated = false;

    for (auto& l : lines) {
        auto p = split(l, '|');
        if (p.size() < 5) continue;

        int id = 0;
        try { id = std::stoi(p[0]); } catch (...) { id = 0; }

        if (id == loanId) {
            std::ostringstream out;
            out << p[0] << "|" << p[1] << "|" << p[2] << "|" << p[3] << "|" << status;
            l = out.str();
            updated = true;
            break;
        }
    }

    if (!updated) return false;
    return writeAllLines(loansFile, lines);
}
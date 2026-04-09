#include "BankSystem.hpp"
#include "Account.hpp"
#include "Transaction.hpp"
#include "Loan.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <vector>
#include <memory>

struct ProfileRow {
    std::string username, mobile, email, address, aadhaar;
};

struct AccountRow {
    int id = 0;
    std::string username;
    std::string accNo;
    std::string type;
    double balance = 0.0;
};

static std::vector<std::string> splitPipe(const std::string& s) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, '|')) out.push_back(item);
    return out;
}

static int nextIdFile(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.is_open()) return 1;
    int mx = 0;
    std::string line;
    while (std::getline(fin, line)) {
        auto p = splitPipe(line);
        if (p.empty()) continue;
        try { mx = std::max(mx, std::stoi(p[0])); } catch (...) {}
    }
    return mx + 1;
}

static std::string makeAccNo(int id) {
    std::ostringstream out;
    out << "AC" << std::setw(6) << std::setfill('0') << id;
    return out.str();
}

static std::vector<ProfileRow> loadProfiles(const std::string& path) {
    std::vector<ProfileRow> out;
    std::ifstream fin(path);
    if (!fin.is_open()) return out;
    std::string line;
    while (std::getline(fin, line)) {
        auto p = splitPipe(line);
        if (p.size() < 5) continue;
        out.push_back({p[0], p[1], p[2], p[3], p[4]});
    }
    return out;
}

static bool saveProfiles(const std::string& path, const std::vector<ProfileRow>& rows) {
    std::ofstream fout(path, std::ios::trunc);
    if (!fout.is_open()) return false;
    for (const auto& r : rows) {
        fout << r.username << "|" << r.mobile << "|" << r.email << "|" << r.address << "|" << r.aadhaar << "\n";
    }
    return true;
}

static bool upsertProfile(const std::string& path, const ProfileRow& row) {
    auto rows = loadProfiles(path);
    bool found = false;
    for (auto& r : rows) {
        if (r.username == row.username) { r = row; found = true; break; }
    }
    if (!found) rows.push_back(row);
    return saveProfiles(path, rows);
}

static bool getProfile(const std::string& path, const std::string& username, ProfileRow& out) {
    auto rows = loadProfiles(path);
    for (const auto& r : rows) {
        if (r.username == username) { out = r; return true; }
    }
    return false;
}

static std::vector<AccountRow> loadAccounts(const std::string& path) {
    std::vector<AccountRow> out;
    std::ifstream fin(path);
    if (!fin.is_open()) return out;
    std::string line;
    while (std::getline(fin, line)) {
        auto p = splitPipe(line);
        if (p.size() < 5) continue;
        AccountRow a;
        try { a.id = std::stoi(p[0]); } catch (...) { continue; }
        a.username = p[1];
        a.accNo = p[2];
        a.type = p[3];
        try { a.balance = std::stod(p[4]); } catch (...) { a.balance = 0.0; }
        out.push_back(a);
    }
    return out;
}

static bool saveAccounts(const std::string& path, const std::vector<AccountRow>& rows) {
    std::ofstream fout(path, std::ios::trunc);
    if (!fout.is_open()) return false;
    for (const auto& a : rows) {
        fout << a.id << "|" << a.username << "|" << a.accNo << "|" << a.type << "|"
             << std::fixed << std::setprecision(2) << a.balance << "\n";
    }
    return true;
}

static bool appendTx(const std::string& path, const std::string& type, const std::string& fromAcc, const std::string& toAcc, double amount, const std::string& ts) {
    int id = nextIdFile(path);
    std::ofstream fout(path, std::ios::app);
    if (!fout.is_open()) return false;
    fout << id << "|" << type << "|" << fromAcc << "|" << toAcc << "|"
         << std::fixed << std::setprecision(2) << amount << "|" << ts << "\n";
    return true;
}

static std::vector<Transaction> loadTxForAcc(const std::string& path, const std::string& accNo) {
    std::vector<Transaction> out;
    std::ifstream fin(path);
    if (!fin.is_open()) return out;
    std::string line;
    while (std::getline(fin, line)) {
        auto p = splitPipe(line);
        if (p.size() < 6) continue;
        if (p[2] != accNo && p[3] != accNo) continue;
        int id = 0; double amt = 0.0;
        try { id = std::stoi(p[0]); } catch (...) { continue; }
        try { amt = std::stod(p[4]); } catch (...) { amt = 0.0; }
        out.emplace_back(id, p[1], p[2], p[3], amt, p[5]);
    }
    return out;
}

static bool appendLoan(const std::string& path, const std::string& username, double amount, double rate, const std::string& status, const std::string& ts, int& outId) {
    int id = nextIdFile(path);
    outId = id;
    std::ofstream fout(path, std::ios::app);
    if (!fout.is_open()) return false;
    fout << id << "|" << username << "|" << std::fixed << std::setprecision(2) << amount << "|"
         << std::fixed << std::setprecision(2) << rate << "|" << status << "|" << ts << "\n";
    return true;
}

static std::vector<Loan> loadLoans(const std::string& path) {
    std::vector<Loan> out;
    std::ifstream fin(path);
    if (!fin.is_open()) return out;
    std::string line;
    while (std::getline(fin, line)) {
        auto p = splitPipe(line);
        if (p.size() < 6) continue;
        int id = 0; double amt = 0.0; double rate = 0.0;
        try { id = std::stoi(p[0]); } catch (...) { continue; }
        try { amt = std::stod(p[2]); } catch (...) { amt = 0.0; }
        try { rate = std::stod(p[3]); } catch (...) { rate = 0.0; }
        out.emplace_back(id, p[1], amt, rate, p[4], p[5]);
    }
    return out;
}

static bool updateLoanStatusFile(const std::string& path, int loanId, const std::string& status) {
    std::ifstream fin(path);
    if (!fin.is_open()) return false;
    std::vector<std::string> lines;
    std::string line;
    bool updated = false;

    while (std::getline(fin, line)) {
        auto p = splitPipe(line);
        if (p.size() < 6) { lines.push_back(line); continue; }
        int id = 0;
        try { id = std::stoi(p[0]); } catch (...) { lines.push_back(line); continue; }

        if (id == loanId) {
            p[4] = status;
            std::ostringstream out;
            out << p[0] << "|" << p[1] << "|" << p[2] << "|" << p[3] << "|" << p[4] << "|" << p[5];
            lines.push_back(out.str());
            updated = true;
        } else lines.push_back(line);
    }

    fin.close();
    std::ofstream fout(path, std::ios::trunc);
    if (!fout.is_open()) return false;
    for (const auto& l : lines) fout << l << "\n";
    return updated;
}

BankSystem::BankSystem() : db(), currentUser(std::nullopt) {
    db.init();
}

int BankSystem::readInt(const std::string& prompt, int minVal, int maxVal) {
    int x;
    while (true) {
        std::cout << prompt;
        if (std::cin >> x && x >= minVal && x <= maxVal) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }
        std::cout << "Invalid input\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double BankSystem::readDouble(const std::string& prompt, double minVal) {
    double x;
    while (true) {
        std::cout << prompt;
        if (std::cin >> x && x >= minVal) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }
        std::cout << "Invalid input\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string BankSystem::readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    while (s.empty()) {
        std::cout << "Input cannot be empty\n";
        std::cout << prompt;
        std::getline(std::cin, s);
    }
    return s;
}

std::string BankSystem::nowDateTime() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

void BankSystem::run() {
    mainMenu();
}

void BankSystem::mainMenu() {
    while (true) {
        std::cout << "1. Register\n";
        std::cout << "2. Login\n";
        std::cout << "3. Exit\n";
        int c = readInt("Choice: ", 1, 3);
        if (c == 3) return;
        if (c == 1) registerMenu();
        if (c == 2) loginMenu();
    }
}

void BankSystem::registerMenu() {
    std::string username = readLine("Username: ");
    std::string password = readLine("Password: ");
    std::string fullName = readLine("Full name: ");

    std::string mobile = readLine("Mobile: ");
    std::string email = readLine("Email: ");
    std::string address = readLine("Address: ");
    std::string aadhaar = readLine("Aadhaar: ");

    std::string err;
    if (!db.registerUser(username, password, fullName, "USER", err)) {
        std::cout << err << "\n";
        return;
    }

    upsertProfile(profilesFile, {username, mobile, email, address, aadhaar});

    std::cout << "Registered\n";
    std::cout << "Create account\n";
    std::cout << "1. Savings\n";
    std::cout << "2. Current\n";
    int t = readInt("Choice: ", 1, 2);
    std::string type = (t == 1 ? "SAVINGS" : "CURRENT");

    auto accs = loadAccounts(accountsFile);
    int newId = nextIdFile(accountsFile);
    std::string accNo = makeAccNo(newId);
    accs.push_back({newId, username, accNo, type, 500.0});
    saveAccounts(accountsFile, accs);

    std::cout << "Account number: " << accNo << "\n";
    std::cout << "Opening balance: " << std::fixed << std::setprecision(2) << 500.0 << "\n";
}

void BankSystem::loginMenu() {
    std::string username = readLine("Username: ");
    std::string password = readLine("Password: ");

    AuthUser au;
    std::string err;
    if (!db.login(username, password, au, err)) {
        std::cout << err << "\n";
        return;
    }

    User u;
    u.setId(au.id);
    u.setUsername(au.username);
    u.setFullName(au.fullName);
    u.setRole(au.role);

    ProfileRow pr;
    if (getProfile(profilesFile, au.username, pr)) {
        u.setMobile(pr.mobile);
        u.setEmail(pr.email);
        u.setAddress(pr.address);
        u.setAadhaar(pr.aadhaar);
    }

    currentUser = u;

    if (currentUser->getRole() == "ADMIN") adminMenu();
    else userMenu();

    currentUser.reset();
}

void BankSystem::adminMenu() {
    while (true) {
        std::cout << "1. View loans\n";
        std::cout << "2. Update loan status\n";
        std::cout << "3. View transactions by account number\n";
        std::cout << "4. Logout\n";
        int c = readInt("Choice: ", 1, 4);
        if (c == 4) return;

        if (c == 1) {
            auto loans = loadLoans(loansFile);
            if (loans.empty()) { std::cout << "No loans\n"; continue; }
            for (const auto& l : loans) {
                std::cout << "Loan id: " << l.getId() << "\n";
                std::cout << "Username: " << l.getUsername() << "\n";
                std::cout << "Amount: " << std::fixed << std::setprecision(2) << l.getAmount() << "\n";
                std::cout << "Rate: " << std::fixed << std::setprecision(2) << l.getInterestRate() << "\n";
                std::cout << "Status: " << l.getStatus() << "\n";
                std::cout << "Time: " << l.getTimestamp() << "\n\n";
            }
        }

        if (c == 2) {
            int id = readInt("Loan id: ", 1, 1000000000);
            std::string st = readLine("Status (APPROVED or REJECTED): ");
            for (auto& ch : st) ch = (char)std::toupper((unsigned char)ch);
            if (st != "APPROVED" && st != "REJECTED") { std::cout << "Invalid status\n"; continue; }
            if (!updateLoanStatusFile(loansFile, id, st)) std::cout << "Loan not found\n";
            else std::cout << "Updated\n";
        }

        if (c == 3) {
            std::string accNo = readLine("Account number: ");
            auto tx = loadTxForAcc(transactionsFile, accNo);
            if (tx.empty()) { std::cout << "No transactions\n"; continue; }
            for (const auto& t : tx) {
                std::cout << t.getTimestamp() << " " << t.getType() << " "
                          << std::fixed << std::setprecision(2) << t.getAmount()
                          << " From " << t.getFromAcc() << " To " << t.getToAcc() << "\n";
            }
        }
    }
}

void BankSystem::userMenu() {
    while (true) {
        std::cout << "1. View profile\n";
        std::cout << "2. Update profile\n";
        std::cout << "3. List accounts\n";
        std::cout << "4. Open new account\n";
        std::cout << "5. Deposit\n";
        std::cout << "6. Withdraw\n";
        std::cout << "7. Transfer\n";
        std::cout << "8. Transactions\n";
        std::cout << "9. Apply loan\n";
        std::cout << "10. My loans\n";
        std::cout << "11. Logout\n";

        int c = readInt("Choice: ", 1, 11);
        if (c == 11) return;

        if (c == 1) {
            std::cout << "Username: " << currentUser->getUsername() << "\n";
            std::cout << "Name: " << currentUser->getFullName() << "\n";
            std::cout << "Mobile: " << currentUser->getMobile() << "\n";
            std::cout << "Email: " << currentUser->getEmail() << "\n";
            std::cout << "Address: " << currentUser->getAddress() << "\n";
            std::cout << "Aadhaar: " << currentUser->getAadhaar() << "\n";
        }

        if (c == 2) {
            ProfileRow pr;
            pr.username = currentUser->getUsername();
            pr.mobile = readLine("Mobile: ");
            pr.email = readLine("Email: ");
            pr.address = readLine("Address: ");
            pr.aadhaar = readLine("Aadhaar: ");
            upsertProfile(profilesFile, pr);
            currentUser->setMobile(pr.mobile);
            currentUser->setEmail(pr.email);
            currentUser->setAddress(pr.address);
            currentUser->setAadhaar(pr.aadhaar);
            std::cout << "Updated\n";
        }

        if (c == 3) {
            auto accs = loadAccounts(accountsFile);
            bool any = false;
            for (const auto& a : accs) {
                if (a.username != currentUser->getUsername()) continue;
                any = true;
                std::cout << "Account number: " << a.accNo << "\n";
                std::cout << "Type: " << a.type << "\n";
                std::cout << "Balance: " << std::fixed << std::setprecision(2) << a.balance << "\n\n";
            }
            if (!any) std::cout << "No accounts\n";
        }

        auto pickAcc = [&]() -> std::string {
            auto accs = loadAccounts(accountsFile);
            std::vector<AccountRow> mine;
            for (const auto& a : accs) if (a.username == currentUser->getUsername()) mine.push_back(a);
            if (mine.empty()) { std::cout << "No accounts\n"; return ""; }
            for (int i = 0; i < (int)mine.size(); i++) {
                std::cout << (i + 1) << ". " << mine[i].accNo << " " << mine[i].type
                          << " " << std::fixed << std::setprecision(2) << mine[i].balance << "\n";
            }
            int idx = readInt("Select: ", 1, (int)mine.size());
            return mine[idx - 1].accNo;
        };

        if (c == 4) {
            std::cout << "1. Savings\n";
            std::cout << "2. Current\n";
            int t = readInt("Choice: ", 1, 2);
            std::string type = (t == 1 ? "SAVINGS" : "CURRENT");

            auto accs = loadAccounts(accountsFile);
            int newId = nextIdFile(accountsFile);
            std::string accNo = makeAccNo(newId);
            accs.push_back({newId, currentUser->getUsername(), accNo, type, 500.0});
            saveAccounts(accountsFile, accs);
            std::cout << "Account number: " << accNo << "\n";
        }

        if (c == 5 || c == 6) {
            std::string accNo = pickAcc();
            if (accNo.empty()) continue;

            double amt = readDouble("Amount: ", 1.0);
            auto accs = loadAccounts(accountsFile);

            for (auto& a : accs) {
                if (a.accNo != accNo) continue;

                std::unique_ptr<Account> obj;
                if (a.type == "SAVINGS") obj = std::make_unique<SavingsAccount>(*currentUser, a.id, a.accNo, a.balance);
                else obj = std::make_unique<CurrentAccount>(*currentUser, a.id, a.accNo, a.balance);

                if (c == 5) {
                    obj->deposit(amt);
                    a.balance = obj->getBalance();
                    saveAccounts(accountsFile, accs);
                    appendTx(transactionsFile, "DEPOSIT", "", accNo, amt, nowDateTime());
                    std::cout << "Done\n";
                } else {
                    if (!obj->withdraw(amt)) {
                        std::cout << "Insufficient funds\n";
                    } else {
                        a.balance = obj->getBalance();
                        saveAccounts(accountsFile, accs);
                        appendTx(transactionsFile, "WITHDRAW", accNo, "", amt, nowDateTime());
                        std::cout << "Done\n";
                    }
                }
                break;
            }
        }

        if (c == 7) {
            std::string fromAccNo = pickAcc();
            if (fromAccNo.empty()) continue;
            std::string toAccNo = readLine("To account number: ");
            double amt = readDouble("Amount: ", 1.0);

            auto accs = loadAccounts(accountsFile);
            int fromIdx = -1, toIdx = -1;
            for (int i = 0; i < (int)accs.size(); i++) {
                if (accs[i].accNo == fromAccNo) fromIdx = i;
                if (accs[i].accNo == toAccNo) toIdx = i;
            }

            if (toIdx == -1) { std::cout << "Receiver not found\n"; continue; }
            if (fromIdx == toIdx) { std::cout << "Same account transfer not allowed\n"; continue; }

            std::unique_ptr<Account> fromObj;
            if (accs[fromIdx].type == "SAVINGS") fromObj = std::make_unique<SavingsAccount>(*currentUser, accs[fromIdx].id, accs[fromIdx].accNo, accs[fromIdx].balance);
            else fromObj = std::make_unique<CurrentAccount>(*currentUser, accs[fromIdx].id, accs[fromIdx].accNo, accs[fromIdx].balance);

            if (!fromObj->withdraw(amt)) {
                std::cout << "Insufficient funds\n";
                continue;
            }

            accs[fromIdx].balance = fromObj->getBalance();
            accs[toIdx].balance += amt;
            saveAccounts(accountsFile, accs);

            appendTx(transactionsFile, "TRANSFER", fromAccNo, toAccNo, amt, nowDateTime());
            std::cout << "Done\n";
        }

        if (c == 8) {
            std::string accNo = pickAcc();
            if (accNo.empty()) continue;
            auto tx = loadTxForAcc(transactionsFile, accNo);
            if (tx.empty()) { std::cout << "No transactions\n"; continue; }
            for (const auto& t : tx) {
                std::cout << t.getTimestamp() << " " << t.getType() << " "
                          << std::fixed << std::setprecision(2) << t.getAmount()
                          << " From " << t.getFromAcc() << " To " << t.getToAcc() << "\n";
            }
        }

        if (c == 9) {
            double amt = readDouble("Loan amount: ", 1.0);
            int loanId = 0;
            if (!appendLoan(loansFile, currentUser->getUsername(), amt, 7.50, "PENDING", nowDateTime(), loanId)) {
                std::cout << "File error\n";
            } else {
                std::cout << "Loan applied\n";
                std::cout << "Loan id: " << loanId << "\n";
            }
        }

        if (c == 10) {
            auto loans = loadLoans(loansFile);
            bool any = false;
            for (const auto& l : loans) {
                if (l.getUsername() != currentUser->getUsername()) continue;
                any = true;
                std::cout << "Loan id: " << l.getId() << "\n";
                std::cout << "Amount: " << std::fixed << std::setprecision(2) << l.getAmount() << "\n";
                std::cout << "Rate: " << std::fixed << std::setprecision(2) << l.getInterestRate() << "\n";
                std::cout << "Status: " << l.getStatus() << "\n";
                std::cout << "Time: " << l.getTimestamp() << "\n\n";
            }
            if (!any) std::cout << "No loans\n";
        }
    }
}
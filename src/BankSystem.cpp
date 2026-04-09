#include "BankSystem.hpp"
#include <iostream>
#include <limits>
#include <ctime>
#include <sstream>
#include <iomanip>

BankSystem::BankSystem() : db(), currentUser(nullptr) {}

BankSystem::~BankSystem() {
    delete currentUser;
    currentUser = nullptr;
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

std::string BankSystem::readString(const std::string& prompt) {
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

std::string BankSystem::normalizeRole(const std::string& r) {
    std::string out;
    for (char c : r) out.push_back((char)std::toupper((unsigned char)c));
    if (out != "ADMIN") return "USER";
    return out;
}

void BankSystem::run() {
    startMenu();
}

void BankSystem::startMenu() {
    while (true) {
        std::cout << "1. Login\n";
        std::cout << "2. Register\n";
        std::cout << "3. Exit\n";
        int c = readInt("Choice: ", 1, 3);

        if (c == 3) return;

        switch (c) {
            case 1:
                loginMenu();
                break;
            case 2:
                registerMenu();
                break;
        }
    }
}

void BankSystem::loginMenu() {
    std::string uname = readString("Username: ");
    std::string pass = readString("Password: ");

    if (uname == "admin" && pass == "admin") {
        adminMenu();
        return;
    }

    User* u = db.getUser(uname);
    if (!u) {
        std::cout << "Invalid login\n";
        return;
    }
    if (u->getPassword() != pass) {
        delete u;
        std::cout << "Invalid login\n";
        return;
    }

    delete currentUser;
    currentUser = u;

    userMenu();
}

void BankSystem::registerMenu() {
    std::string uname = readString("Username: ");
    if (uname == "admin") {
        std::cout << "Username not allowed\n";
        return;
    }

    User* existing = db.getUser(uname);
    if (existing) {
        delete existing;
        std::cout << "Username already exists\n";
        return;
    }

    std::string pass = readString("Password: ");
    std::string name = readString("Full name: ");
    std::string mobile = readString("Mobile: ");
    std::string email = readString("Email: ");
    std::string address = readString("Address: ");
    std::string aadhaar = readString("Aadhaar: ");
    std::string role = "USER";

    User newUser(uname, pass, name, role, mobile, email, address, aadhaar);
    if (!db.addUser(newUser)) {
        std::cout << "Registration failed\n";
        return;
    }

    std::cout << "Account type\n";
    std::cout << "1. Savings\n";
    std::cout << "2. Current\n";
    std::cout << "3. Fixed deposit\n";
    int t = readInt("Choice: ", 1, 3);

    Account* acc = nullptr;
    if (t == 1) acc = new SavingsAccount(newUser.getId(), newUser.getUsername(), newUser.getFullName());
    else if (t == 2) acc = new CurrentAccount(newUser.getId(), newUser.getUsername(), newUser.getFullName());
    else acc = new FixedDepositAccount(newUser.getId(), newUser.getUsername(), newUser.getFullName());

    bool ok = db.addAccount(*acc);
    if (!ok) {
        delete acc;
        std::cout << "Account creation failed\n";
        return;
    }

    std::cout << "Registered\n";
    std::cout << "Account number: " << acc->getAccountNumber() << "\n";
    std::cout << "Opening balance: " << std::fixed << std::setprecision(2) << acc->getBalance() << "\n";

    delete acc;
}

void BankSystem::adminMenu() {
    while (true) {
        std::cout << "1. View users\n";
        std::cout << "2. View all loans\n";
        std::cout << "3. Update loan status\n";
        std::cout << "4. Logout\n";
        int c = readInt("Choice: ", 1, 4);

        if (c == 4) return;

        switch (c) {
            case 1: adminViewUsers(); break;
            case 2: adminViewAllLoans(); break;
            case 3: adminUpdateLoan(); break;
        }
    }
}

void BankSystem::adminViewUsers() {
    auto users = db.getAllUsers();
    if (users.empty()) {
        std::cout << "No users\n";
        return;
    }

    for (auto u : users) {
        std::cout << "Id: " << u->getId() << "\n";
        std::cout << "Username: " << u->getUsername() << "\n";
        std::cout << "Name: " << u->getFullName() << "\n";
        std::cout << "Mobile: " << u->getMobile() << "\n";
        std::cout << "Email: " << u->getEmail() << "\n";
        std::cout << "\n";
    }

    for (auto u : users) delete u;
}

void BankSystem::adminViewAllLoans() {
    auto loans = db.getAllLoans();
    if (loans.empty()) {
        std::cout << "No loans\n";
        return;
    }

    for (auto l : loans) {
        std::cout << "Loan id: " << l->getId() << "\n";
        std::cout << "User id: " << l->getUserId() << "\n";
        std::cout << "Amount: " << std::fixed << std::setprecision(2) << l->getAmount() << "\n";
        std::cout << "Rate: " << std::fixed << std::setprecision(2) << l->getInterestRate() << "\n";
        std::cout << "Status: " << l->getStatus() << "\n";
        std::cout << "\n";
    }

    for (auto l : loans) delete l;
}

void BankSystem::adminUpdateLoan() {
    int loanId = readInt("Loan id: ", 1, 1000000000);
    std::string status = readString("Status (APPROVED or REJECTED): ");
    for (auto& c : status) c = (char)std::toupper((unsigned char)c);

    if (status != "APPROVED" && status != "REJECTED") {
        std::cout << "Invalid status\n";
        return;
    }

    if (!db.updateLoanStatus(loanId, status)) {
        std::cout << "Loan not found\n";
        return;
    }

    std::cout << "Updated\n";
}

void BankSystem::userMenu() {
    while (true) {
        std::cout << "1. View accounts\n";
        std::cout << "2. Deposit\n";
        std::cout << "3. Withdraw\n";
        std::cout << "4. View profile\n";
        std::cout << "5. View transactions\n";
        std::cout << "6. Apply loan\n";
        std::cout << "7. View my loans\n";
        std::cout << "8. Logout\n";
        int c = readInt("Choice: ", 1, 8);

        if (c == 8) {
            delete currentUser;
            currentUser = nullptr;
            return;
        }

        switch (c) {
            case 1: userViewAccounts(); break;
            case 2: userDeposit(); break;
            case 3: userWithdraw(); break;
            case 4: userViewProfile(); break;
            case 5: userViewTransactions(); break;
            case 6: userApplyLoan(); break;
            case 7: userViewLoans(); break;
        }
    }
}

void BankSystem::userViewAccounts() {
    auto accs = db.getUserAccounts(*currentUser);
    if (accs.empty()) {
        std::cout << "No accounts\n";
        return;
    }

    for (auto a : accs) {
        std::cout << "Account id: " << a->getId() << "\n";
        std::cout << "Account number: " << a->getAccountNumber() << "\n";
        std::cout << "Type: " << a->getAccountType() << "\n";
        std::cout << "Balance: " << std::fixed << std::setprecision(2) << a->getBalance() << "\n";
        std::cout << "\n";
    }

    for (auto a : accs) delete a;
}

Account* BankSystem::pickAccountFromUser() const {
    auto accs = db.getUserAccounts(*currentUser);
    if (accs.empty()) {
        std::cout << "No accounts\n";
        return nullptr;
    }

    for (int i = 0; i < (int)accs.size(); i++) {
        std::cout << (i + 1) << ". " << accs[i]->getAccountNumber()
                  << " " << accs[i]->getAccountType()
                  << " " << std::fixed << std::setprecision(2) << accs[i]->getBalance()
                  << "\n";
    }

    int idx = readInt("Select account: ", 1, (int)accs.size());

    Account* selected = accs[idx - 1];
    accs[idx - 1] = nullptr;

    for (auto a : accs) delete a;
    return selected;
}

void BankSystem::userDeposit() {
    Account* acc = pickAccountFromUser();
    if (!acc) return;

    double amt = readDouble("Amount: ", 1.0);
    acc->deposit(amt);

    db.updateBalance(acc->getId(), acc->getBalance());

    Transaction t(acc->getId(), "DEPOSIT", amt, nowDateTime());
    db.addTransaction(t);

    std::cout << "Deposit done\n";
    delete acc;
}

void BankSystem::userWithdraw() {
    Account* acc = pickAccountFromUser();
    if (!acc) return;

    double amt = readDouble("Amount: ", 1.0);

    if (!acc->withdraw(amt)) {
        if (acc->getAccountType() == "FD") std::cout << "Withdraw not allowed for fixed deposit\n";
        else std::cout << "Insufficient balance\n";
        delete acc;
        return;
    }

    db.updateBalance(acc->getId(), acc->getBalance());

    Transaction t(acc->getId(), "WITHDRAW", amt, nowDateTime());
    db.addTransaction(t);

    std::cout << "Withdraw done\n";
    delete acc;
}

void BankSystem::userViewProfile() {
    std::cout << "Id: " << currentUser->getId() << "\n";
    std::cout << "Username: " << currentUser->getUsername() << "\n";
    std::cout << "Name: " << currentUser->getFullName() << "\n";
    std::cout << "Mobile: " << currentUser->getMobile() << "\n";
    std::cout << "Email: " << currentUser->getEmail() << "\n";
    std::cout << "Address: " << currentUser->getAddress() << "\n";
    std::cout << "Aadhaar: " << currentUser->getAadhaar() << "\n";
}

void BankSystem::userViewTransactions() {
    Account* acc = pickAccountFromUser();
    if (!acc) return;

    auto tx = db.getAccountTransactions(acc->getId());
    if (tx.empty()) {
        std::cout << "No transactions\n";
        delete acc;
        return;
    }

    for (auto t : tx) {
        std::cout << t->getDateTime() << " " << t->getType() << " " << std::fixed << std::setprecision(2) << t->getAmount() << "\n";
    }

    for (auto t : tx) delete t;
    delete acc;
}

void BankSystem::userApplyLoan() {
    double amt = readDouble("Loan amount: ", 1.0);
    double rate = 7.50;

    Loan l(currentUser->getId(), amt, rate, "PENDING");
    if (!db.addLoan(l)) {
        std::cout << "Loan apply failed\n";
        return;
    }

    std::cout << "Loan applied\n";
    std::cout << "Loan id: " << l.getId() << "\n";
}

void BankSystem::userViewLoans() {
    auto loans = db.getUserLoans(currentUser->getId());
    if (loans.empty()) {
        std::cout << "No loans\n";
        return;
    }

    for (auto l : loans) {
        std::cout << "Loan id: " << l->getId() << "\n";
        std::cout << "Amount: " << std::fixed << std::setprecision(2) << l->getAmount() << "\n";
        std::cout << "Rate: " << std::fixed << std::setprecision(2) << l->getInterestRate() << "\n";
        std::cout << "Status: " << l->getStatus() << "\n";
        std::cout << "\n";
    }

    for (auto l : loans) delete l;
}
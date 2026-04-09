#include "BankSystem.hpp"
#include <iostream>
#include <limits>
#include <iomanip>

BankSystem::BankSystem() : db(), currentUser(std::nullopt) {}

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

std::string BankSystem::toUpper(std::string s) {
    for (char& c : s) c = (char)std::toupper((unsigned char)c);
    return s;
}

void BankSystem::run() {
    mainMenu();
}

void BankSystem::mainMenu() {
    while (true) {
        std::cout << "1. Login\n";
        std::cout << "2. Register\n";
        std::cout << "3. Exit\n";
        int c = readInt("Choice: ", 1, 3);

        if (c == 3) return;

        switch (c) {
            case 1: login(); break;
            case 2: registerUser(); break;
        }
    }
}

void BankSystem::login() {
    std::string username = readLine("Username: ");
    std::string password = readLine("Password: ");

    User u;
    std::string err;
    if (!db.authenticate(username, password, u, err)) {
        std::cout << err << "\n";
        return;
    }

    currentUser = u;

    if (currentUser->getRole() == "ADMIN") adminMenu();
    else userMenu();

    currentUser.reset();
}

void BankSystem::registerUser() {
    std::string username = readLine("Username: ");
    std::string password = readLine("Password: ");
    std::string fullName = readLine("Full name: ");

    std::string err;
    if (!db.registerUser(username, password, fullName, err)) {
        std::cout << err << "\n";
        return;
    }

    std::cout << "User registered\n";
    std::cout << "Create first account\n";
    userOpenAccount();
}

void BankSystem::adminMenu() {
    while (true) {
        std::cout << "1. View users\n";
        std::cout << "2. View accounts\n";
        std::cout << "3. View account transactions\n";
        std::cout << "4. Logout\n";
        int c = readInt("Choice: ", 1, 4);
        if (c == 4) return;

        switch (c) {
            case 1: adminViewUsers(); break;
            case 2: adminViewAccounts(); break;
            case 3: adminViewAccountTransactions(); break;
        }
    }
}

void BankSystem::userMenu() {
    while (true) {
        std::cout << "1. List accounts\n";
        std::cout << "2. Open new account\n";
        std::cout << "3. Deposit\n";
        std::cout << "4. Withdraw\n";
        std::cout << "5. Transfer\n";
        std::cout << "6. Transactions\n";
        std::cout << "7. Logout\n";

        int c = readInt("Choice: ", 1, 7);
        if (c == 7) return;

        switch (c) {
            case 1: userListAccounts(); break;
            case 2: userOpenAccount(); break;
            case 3: userDeposit(); break;
            case 4: userWithdraw(); break;
            case 5: userTransfer(); break;
            case 6: userTransactions(); break;
        }
    }
}

void BankSystem::adminViewUsers() {
    auto users = db.getAllUsers();
    if (users.empty()) {
        std::cout << "No users\n";
        return;
    }

    for (const auto& u : users) {
        std::cout << "Id: " << u.getId() << "\n";
        std::cout << "Username: " << u.getUsername() << "\n";
        std::cout << "Name: " << u.getFullName() << "\n";
        std::cout << "Role: " << u.getRole() << "\n";
        std::cout << "\n";
    }
}

void BankSystem::adminViewAccounts() {
    auto accs = db.getAllAccounts();
    if (accs.empty()) {
        std::cout << "No accounts\n";
        return;
    }

    for (const auto& a : accs) {
        std::cout << "Account id: " << a->getAccountId() << "\n";
        std::cout << "Account number: " << a->getAccountNumber() << "\n";
        std::cout << "Type: " << a->getAccountType() << "\n";
        std::cout << "User id: " << a->getUserId() << "\n";
        std::cout << "Username: " << a->getUsername() << "\n";
        std::cout << "Name: " << a->getFullName() << "\n";
        std::cout << "Balance: " << std::fixed << std::setprecision(2) << a->getBalance() << "\n";
        std::cout << "\n";
    }
}

void BankSystem::adminViewAccountTransactions() {
    std::string accNo = readLine("Account number: ");
    std::unique_ptr<Account> acc;
    if (!db.getAccountByNumber(accNo, acc)) {
        std::cout << "Account not found\n";
        return;
    }

    auto tx = db.getTransactionsForAccount(acc->getAccountId());
    if (tx.empty()) {
        std::cout << "No transactions\n";
        return;
    }

    for (const auto& t : tx) {
        std::cout << t.getTimestamp() << " ";
        std::cout << t.getType() << " ";
        std::cout << std::fixed << std::setprecision(2) << t.getAmount() << " ";
        std::cout << "From " << t.getFromAccountId() << " ";
        std::cout << "To " << t.getToAccountId() << "\n";
    }
}

void BankSystem::userListAccounts() {
    auto accs = db.getAccountsForUser(currentUser->getId());
    if (accs.empty()) {
        std::cout << "No accounts\n";
        return;
    }

    for (const auto& a : accs) {
        std::cout << "Account id: " << a->getAccountId() << "\n";
        std::cout << "Account number: " << a->getAccountNumber() << "\n";
        std::cout << "Type: " << a->getAccountType() << "\n";
        std::cout << "Balance: " << std::fixed << std::setprecision(2) << a->getBalance() << "\n";
        std::cout << "\n";
    }
}

void BankSystem::userOpenAccount() {
    if (!currentUser.has_value()) {
        std::cout << "Login required\n";
        return;
    }

    std::cout << "1. Savings\n";
    std::cout << "2. Current\n";
    int t = readInt("Choice: ", 1, 2);

    std::string type = (t == 1 ? "SAVINGS" : "CURRENT");
    std::string accNo;
    std::string err;

    if (!db.createAccount(currentUser->getId(), type, accNo, err)) {
        std::cout << err << "\n";
        return;
    }

    std::cout << "Account created\n";
    std::cout << "Account number: " << accNo << "\n";
    std::cout << "Opening balance: " << std::fixed << std::setprecision(2) << 500.0 << "\n";
}

bool BankSystem::pickUserAccount(std::unique_ptr<Account>& outAccount) const {
    outAccount.reset();
    auto accs = db.getAccountsForUser(currentUser->getId());
    if (accs.empty()) {
        std::cout << "No accounts\n";
        return false;
    }

    for (int i = 0; i < (int)accs.size(); i++) {
        std::cout << (i + 1) << ". "
                  << accs[i]->getAccountNumber() << " "
                  << accs[i]->getAccountType() << " "
                  << std::fixed << std::setprecision(2) << accs[i]->getBalance()
                  << "\n";
    }

    int idx = readInt("Select: ", 1, (int)accs.size());
    outAccount = std::move(accs[idx - 1]);
    return true;
}

void BankSystem::userDeposit() {
    std::unique_ptr<Account> acc;
    if (!pickUserAccount(acc)) return;

    double amt = readDouble("Amount: ", 1.0);

    std::string err;
    if (!db.deposit(acc->getAccountId(), amt, err)) {
        std::cout << err << "\n";
        return;
    }

    std::cout << "Deposit done\n";
}

void BankSystem::userWithdraw() {
    std::unique_ptr<Account> acc;
    if (!pickUserAccount(acc)) return;

    double amt = readDouble("Amount: ", 1.0);

    std::string err;
    if (!db.withdraw(acc->getAccountId(), amt, err)) {
        std::cout << err << "\n";
        return;
    }

    std::cout << "Withdraw done\n";
}

void BankSystem::userTransfer() {
    std::unique_ptr<Account> fromAcc;
    if (!pickUserAccount(fromAcc)) return;

    std::string toAccNo = readLine("To account number: ");
    double amt = readDouble("Amount: ", 1.0);

    std::string err;
    if (!db.transfer(fromAcc->getAccountId(), toAccNo, amt, err)) {
        std::cout << err << "\n";
        return;
    }

    std::cout << "Transfer done\n";
}

void BankSystem::userTransactions() {
    std::unique_ptr<Account> acc;
    if (!pickUserAccount(acc)) return;

    auto tx = db.getTransactionsForAccount(acc->getAccountId());
    if (tx.empty()) {
        std::cout << "No transactions\n";
        return;
    }

    for (const auto& t : tx) {
        std::cout << t.getTimestamp() << " ";
        std::cout << t.getType() << " ";
        std::cout << std::fixed << std::setprecision(2) << t.getAmount() << " ";
        std::cout << "From " << t.getFromAccountId() << " ";
        std::cout << "To " << t.getToAccountId() << "\n";
    }
}
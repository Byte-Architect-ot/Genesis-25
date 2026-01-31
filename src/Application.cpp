#include "Application.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <iostream>

//Application class implementationn
Application::Application() 
    : window(sf::VideoMode(1000, 700), "Simple Bank System"),
      database("data/bank.db"),
      currentUser(nullptr),
      currentScreen(Screen::LOGIN),
      isError(false),
      selectedAccount(0) {
    
    // Set frame rate limit
    window.setFramerateLimit(60);
    
    // Initialize ImGui with SFML
    if (!ImGui::SFML::Init(window)) {
        throw std::runtime_error("Failed to initialize ImGui");
    }
    
    // Make text bigger and easier to read
    ImGui::GetIO().FontGlobalScale = 1.3f;
    
    // Use dark theme
    ImGui::StyleColorsDark();
    
    std::cout << "Application started" << std::endl;
}

Application::~Application() {
    ImGui::SFML::Shutdown();
    std::cout << "Application closed" << std::endl;
}

void Application::run() {
    // Main application loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        
        // Update ImGui
        ImGui::SFML::Update(window, clock.restart());
        
        // Show current screen
        switch (currentScreen) {
            case Screen::LOGIN:
                showLoginScreen();
                break;
            case Screen::REGISTER:
                showRegisterScreen();
                break;
            case Screen::DASHBOARD:
                showDashboard();
                break;
            case Screen::DEPOSIT:
                showDepositScreen();
                break;
            case Screen::WITHDRAW:
                showWithdrawScreen();
                break;
            case Screen::TRANSFER:
                showTransferScreen();
                break;
            case Screen::HISTORY:
                showHistoryScreen();
                break;
            case Screen::NEW_ACCOUNT:
                showNewAccountScreen();
                break;
        }
        
        // Render
        window.clear(sf::Color(40, 44, 52));  // Dark background
        ImGui::SFML::Render(window);
        window.display();
    }
}

//Helper functions in Application Class
void Application::clearInputs() {
    memset(usernameInput, 0, sizeof(usernameInput));
    memset(passwordInput, 0, sizeof(passwordInput));
    memset(confirmPasswordInput, 0, sizeof(confirmPasswordInput));
    memset(nameInput, 0, sizeof(nameInput));
    memset(amountInput, 0, sizeof(amountInput));
    memset(targetAccountInput, 0, sizeof(targetAccountInput));
}

void Application::setStatus(const std::string& message, bool error) {
    statusMessage = message;
    isError = error;
}

void Application::refreshUserAccounts() {
    if (currentUser) {
        currentUser->getAccounts().clear();
        auto accounts = database.getUserAccounts(currentUser->getId());
        for (auto& acc : accounts) {
            currentUser->addAccount(acc);
        }
    }
}

std::string Application::formatMoney(double amount) {
    std::stringstream ss;
    ss << "Rs. " << std::fixed << std::setprecision(2) << amount;
    return ss.str();
}

//First you are going to see this, basically a login screen where you will have to login to your account
void Application::showLoginScreen() {
    // Center the window
    ImGui::SetNextWindowPos(ImVec2(350, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 280));
    
    ImGui::Begin("Bank Login", nullptr, 
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("Welcome to WiseVault!");
    ImGui::Separator();
    ImGui::Spacing();
    
    // Username input
    ImGui::Text("Username:");
    ImGui::InputText("##username", usernameInput, sizeof(usernameInput));
    
    ImGui::Spacing();
    
    // Password input (hidden)
    ImGui::Text("Password:");
    ImGui::InputText("##password", passwordInput, sizeof(passwordInput), 
                     ImGuiInputTextFlags_Password);
    
    ImGui::Spacing();
    
    // Show status message
    if (!statusMessage.empty()) {
        if (isError) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "%s", statusMessage.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 1, 0.3f, 1), "%s", statusMessage.c_str());
        }
    }
    
    ImGui::Spacing();
    
    // Login button
    if (ImGui::Button("Login", ImVec2(130, 35))) {
        // Check if fields are empty
        if (strlen(usernameInput) == 0 || strlen(passwordInput) == 0) {
            setStatus("Please enter username and password", true);
        } else {
            // Try to get user from database
            auto user = database.getUser(usernameInput);
            
            if (user && User::verifyPassword(passwordInput, user->getPassword())) {
                // Login successful
                currentUser = std::make_shared<User>(*user);
                refreshUserAccounts();
                clearInputs();
                setStatus("");
                currentScreen = Screen::DASHBOARD;
                std::cout << "User logged in: " << currentUser->getUsername() << std::endl;
            } else {
                setStatus("Invalid username or password", true);
            }
        }
    }
    
    ImGui::SameLine();
    
    // Register button
    if (ImGui::Button("Register", ImVec2(130, 35))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::REGISTER;
    }
    
    ImGui::End();
}

//If you have to register? here's its implementation
void Application::showRegisterScreen() {
    ImGui::SetNextWindowPos(ImVec2(300, 150), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 380));
    
    ImGui::Begin("Create Account", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("Register New User");
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Full Name:");
    ImGui::InputText("##name", nameInput, sizeof(nameInput));
    
    ImGui::Spacing();
    
    ImGui::Text("Username:");
    ImGui::InputText("##username", usernameInput, sizeof(usernameInput));
    
    ImGui::Spacing();
    
    ImGui::Text("Password:");
    ImGui::InputText("##password", passwordInput, sizeof(passwordInput),
                     ImGuiInputTextFlags_Password);
    
    ImGui::Spacing();
    
    ImGui::Text("Confirm Password:");
    ImGui::InputText("##confirm", confirmPasswordInput, sizeof(confirmPasswordInput),
                     ImGuiInputTextFlags_Password);
    
    ImGui::Spacing();
    
    // Show status
    if (!statusMessage.empty()) {
        if (isError) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "%s", statusMessage.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 1, 0.3f, 1), "%s", statusMessage.c_str());
        }
    }
    
    ImGui::Spacing();
    
    // Register button
    if (ImGui::Button("Create Account", ImVec2(180, 35))) {
        // Validate inputs
        if (strlen(nameInput) == 0 || strlen(usernameInput) == 0 || 
            strlen(passwordInput) == 0) {
            setStatus("Please fill all fields", true);
        } else if (strlen(passwordInput) < 6) {
            setStatus("Password must be at least 6 characters", true);
        } else if (strcmp(passwordInput, confirmPasswordInput) != 0) {
            setStatus("Passwords don't match", true);
        } else if (database.userExists(usernameInput)) {
            setStatus("Username already exists", true);
        } else {
            // Create new user
            User newUser(usernameInput, passwordInput, nameInput);
            
            if (database.addUser(newUser)) {
                // Create default savings account
                SavingsAccount account(newUser.getId());
                database.addAccount(account);
                
                // Record initial deposit
                database.addTransaction(account.getId(), "DEPOSIT", 
                                        500.0, 500.0, "Opening deposit");
                
                clearInputs();
                setStatus("Account created! Please login.");
                currentScreen = Screen::LOGIN;
            } else {
                setStatus("Failed to create account", true);
            }
        }
    }
    
    ImGui::SameLine();
    
    // Back button
    if (ImGui::Button("Back", ImVec2(180, 35))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::LOGIN;
    }
    
    ImGui::End();
}

//THis on is our main dashboard screen, all you are going see here
void Application::showDashboard() {
    if (!currentUser) {
        currentScreen = Screen::LOGIN;
        return;
    }
    
    // Refresh account data
    refreshUserAccounts();
    
    // Full window dashboard
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    ImGui::SetNextWindowSize(ImVec2(960, 660));
    
    ImGui::Begin("Dashboard", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // Header
    ImGui::Text("Welcome, %s!", currentUser->getFullName().c_str());
    
    ImGui::SameLine(800);
    if (ImGui::Button("Logout", ImVec2(100, 30))) {
        currentUser = nullptr;
        clearInputs();
        setStatus("");
        currentScreen = Screen::LOGIN;
        return;
    }
    
    ImGui::Separator();
    ImGui::Spacing();
    
    // Account Summary
    ImGui::Text("Your Accounts:");
    ImGui::Spacing();
    
    // Table of accounts
    ImGui::BeginChild("AccountList", ImVec2(0, 200), true);
    
    // Table headers
    ImGui::Columns(4, "accounts");
    ImGui::SetColumnWidth(0, 200);
    ImGui::SetColumnWidth(1, 150);
    ImGui::SetColumnWidth(2, 200);
    ImGui::SetColumnWidth(3, 150);
    
    ImGui::Text("Account Number");
    ImGui::NextColumn();
    ImGui::Text("Type");
    ImGui::NextColumn();
    ImGui::Text("Balance");
    ImGui::NextColumn();
    ImGui::Text("Status");
    ImGui::NextColumn();
    ImGui::Separator();
    
    // Account rows
    auto& accounts = currentUser->getAccounts();
    for (const auto& acc : accounts) {
        ImGui::Text("%s", acc->getAccountNumber().c_str());
        ImGui::NextColumn();
        
        ImGui::Text("%s", acc->getAccountType().c_str());
        ImGui::NextColumn();
        
        // Show balance in green
        ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1), 
                          "%s", formatMoney(acc->getBalance()).c_str());
        ImGui::NextColumn();
        
        ImGui::Text("Active");
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
    ImGui::EndChild();
    
    // Total balance
    ImGui::Spacing();
    ImGui::Text("Total Balance: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1), "%s", formatMoney(currentUser->getTotalBalance()).c_str());
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Action buttons
    ImGui::Text("Quick Actions:");
    ImGui::Spacing();
    
    if (ImGui::Button("Deposit", ImVec2(120, 45))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::DEPOSIT;
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Withdraw", ImVec2(120, 45))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::WITHDRAW;
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Transfer", ImVec2(120, 45))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::TRANSFER;
    }
    ImGui::SameLine();
    
    if (ImGui::Button("History", ImVec2(120, 45))) {
        setStatus("");
        currentScreen = Screen::HISTORY;
    }
    ImGui::SameLine();
    
    if (ImGui::Button("New Account", ImVec2(120, 45))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::NEW_ACCOUNT;
    }
    
    ImGui::End();
}

//In case of deposit any xyz amount
void Application::showDepositScreen() {
    ImGui::SetNextWindowPos(ImVec2(350, 200));
    ImGui::SetNextWindowSize(ImVec2(300, 280));
    
    ImGui::Begin("Deposit", nullptr, ImGuiWindowFlags_NoCollapse);
    
    auto& accounts = currentUser->getAccounts();
    
    ImGui::Text("Select Account:");
    
    // Account dropdown
    if (!accounts.empty()) {
        if (selectedAccount >= static_cast<int>(accounts.size())) {
            selectedAccount = 0;
        }
        
        std::string preview = accounts[selectedAccount]->getAccountNumber() + " - " +
                             formatMoney(accounts[selectedAccount]->getBalance());
        
        if (ImGui::BeginCombo("##account", preview.c_str())) {
            for (int i = 0; i < static_cast<int>(accounts.size()); i++) {
                std::string label = accounts[i]->getAccountNumber() + " - " +
                                   formatMoney(accounts[i]->getBalance());
                if (ImGui::Selectable(label.c_str(), selectedAccount == i)) {
                    selectedAccount = i;
                }
            }
            ImGui::EndCombo();
        }
    }
    
    ImGui::Spacing();
    ImGui::Text("Amount:");
    ImGui::InputText("##amount", amountInput, sizeof(amountInput));
    
    ImGui::Spacing();
    
    // Status message
    if (!statusMessage.empty()) {
        if (isError) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "%s", statusMessage.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 1, 0.3f, 1), "%s", statusMessage.c_str());
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::Button("Deposit", ImVec2(130, 35))) {
        double amount = atof(amountInput);
        
        if (amount <= 0) {
            setStatus("Enter a valid amount", true);
        } else if (!accounts.empty()) {
            auto& acc = accounts[selectedAccount];
            double newBalance = acc->getBalance() + amount;
            
            // Update database
            if (database.updateBalance(acc->getId(), newBalance)) {
                database.addTransaction(acc->getId(), "DEPOSIT", 
                                       amount, newBalance, "Cash deposit");
                acc->setBalance(newBalance);
                memset(amountInput, 0, sizeof(amountInput));
                setStatus("Deposit successful!");
            } else {
                setStatus("Deposit failed", true);
            }
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Back", ImVec2(130, 35))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::DASHBOARD;
    }
    
    ImGui::End();
}

//To withdraw
void Application::showWithdrawScreen() {
    ImGui::SetNextWindowPos(ImVec2(350, 200));
    ImGui::SetNextWindowSize(ImVec2(300, 300));
    
    ImGui::Begin("Withdraw", nullptr, ImGuiWindowFlags_NoCollapse);
    
    auto& accounts = currentUser->getAccounts();
    
    ImGui::Text("Select Account:");
    
    if (!accounts.empty()) {
        if (selectedAccount >= static_cast<int>(accounts.size())) {
            selectedAccount = 0;
        }
        
        std::string preview = accounts[selectedAccount]->getAccountNumber() + " - " +
                             formatMoney(accounts[selectedAccount]->getBalance());
        
        if (ImGui::BeginCombo("##account", preview.c_str())) {
            for (int i = 0; i < static_cast<int>(accounts.size()); i++) {
                std::string label = accounts[i]->getAccountNumber() + " - " +
                                   formatMoney(accounts[i]->getBalance());
                if (ImGui::Selectable(label.c_str(), selectedAccount == i)) {
                    selectedAccount = i;
                }
            }
            ImGui::EndCombo();
        }
        
        // Show available balance
        ImGui::Text("Available: %s", 
                   formatMoney(accounts[selectedAccount]->getBalance()).c_str());
    }
    
    ImGui::Spacing();
    ImGui::Text("Amount:");
    ImGui::InputText("##amount", amountInput, sizeof(amountInput));
    
    ImGui::Spacing();
    
    if (!statusMessage.empty()) {
        if (isError) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "%s", statusMessage.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 1, 0.3f, 1), "%s", statusMessage.c_str());
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::Button("Withdraw", ImVec2(130, 35))) {
        double amount = atof(amountInput);
        
        if (amount <= 0) {
            setStatus("Enter a valid amount", true);
        } else if (!accounts.empty()) {
            auto& acc = accounts[selectedAccount];
            
            // Check if can withdraw
            if (!acc->canWithdraw(amount)) {
                if (acc->getAccountType() == "SAVINGS") {
                    setStatus("Must maintain Rs. 500 minimum balance", true);
                } else {
                    setStatus("Insufficient balance", true);
                }
            } else {
                double newBalance = acc->getBalance() - amount;
                
                if (database.updateBalance(acc->getId(), newBalance)) {
                    database.addTransaction(acc->getId(), "WITHDRAW",
                                           amount, newBalance, "Cash withdrawal");
                    acc->setBalance(newBalance);
                    memset(amountInput, 0, sizeof(amountInput));
                    setStatus("Withdrawal successful!");
                } else {
                    setStatus("Withdrawal failed", true);
                }
            }
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Back", ImVec2(130, 35))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::DASHBOARD;
    }
    
    ImGui::End();
}

//Transfer like bank to bank
void Application::showTransferScreen() {
    ImGui::SetNextWindowPos(ImVec2(300, 150));
    ImGui::SetNextWindowSize(ImVec2(400, 350));
    
    ImGui::Begin("Transfer Money", nullptr, ImGuiWindowFlags_NoCollapse);
    
    auto& accounts = currentUser->getAccounts();
    
    ImGui::Text("From Account:");
    
    if (!accounts.empty()) {
        if (selectedAccount >= static_cast<int>(accounts.size())) {
            selectedAccount = 0;
        }
        
        std::string preview = accounts[selectedAccount]->getAccountNumber() + " - " +
                             formatMoney(accounts[selectedAccount]->getBalance());
        
        if (ImGui::BeginCombo("##fromaccount", preview.c_str())) {
            for (int i = 0; i < static_cast<int>(accounts.size()); i++) {
                std::string label = accounts[i]->getAccountNumber() + " - " +
                                   formatMoney(accounts[i]->getBalance());
                if (ImGui::Selectable(label.c_str(), selectedAccount == i)) {
                    selectedAccount = i;
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Available: %s", 
                   formatMoney(accounts[selectedAccount]->getBalance()).c_str());
    }
    
    ImGui::Spacing();
    ImGui::Text("To Account Number:");
    ImGui::InputText("##toaccount", targetAccountInput, sizeof(targetAccountInput));
    
    ImGui::Spacing();
    ImGui::Text("Amount:");
    ImGui::InputText("##amount", amountInput, sizeof(amountInput));
    
    ImGui::Spacing();
    
    if (!statusMessage.empty()) {
        if (isError) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "%s", statusMessage.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 1, 0.3f, 1), "%s", statusMessage.c_str());
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::Button("Transfer", ImVec2(180, 35))) {
        double amount = atof(amountInput);
        std::string targetAcc = targetAccountInput;
        
        if (amount <= 0) {
            setStatus("Enter a valid amount", true);
        } else if (targetAcc.empty()) {
            setStatus("Enter target account number", true);
        } else if (!accounts.empty()) {
            auto& sourceAcc = accounts[selectedAccount];
            
            // Check if same account
            if (sourceAcc->getAccountNumber() == targetAcc) {
                setStatus("Cannot transfer to same account", true);
            }
            // Check if can withdraw
            else if (!sourceAcc->canWithdraw(amount)) {
                setStatus("Insufficient balance", true);
            } else {
                // Find target account in database
                auto destAcc = database.getAccountByNumber(targetAcc);
                
                if (!destAcc) {
                    setStatus("Target account not found", true);
                } else {
                    // Do the transfer
                    double newSourceBalance = sourceAcc->getBalance() - amount;
                    double newDestBalance = destAcc->getBalance() + amount;
                    
                    // Update both accounts
                    bool success = database.updateBalance(sourceAcc->getId(), newSourceBalance);
                    success = success && database.updateBalance(destAcc->getId(), newDestBalance);
                    
                    if (success) {
                        // Record transactions
                        database.addTransaction(sourceAcc->getId(), "TRANSFER_OUT",
                                               amount, newSourceBalance, 
                                               "Transfer to " + targetAcc);
                        database.addTransaction(destAcc->getId(), "TRANSFER_IN",
                                               amount, newDestBalance,
                                               "Transfer from " + sourceAcc->getAccountNumber());
                        
                        sourceAcc->setBalance(newSourceBalance);
                        
                        memset(amountInput, 0, sizeof(amountInput));
                        memset(targetAccountInput, 0, sizeof(targetAccountInput));
                        setStatus("Transfer successful!");
                    } else {
                        setStatus("Transfer failed", true);
                    }
                }
            }
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Back", ImVec2(180, 35))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::DASHBOARD;
    }
    
    ImGui::End();
}

//TO videw the history (transaction history)
void Application::showHistoryScreen() {
    ImGui::SetNextWindowPos(ImVec2(100, 50));
    ImGui::SetNextWindowSize(ImVec2(800, 600));
    
    ImGui::Begin("Transaction History", nullptr, ImGuiWindowFlags_NoCollapse);
    
    auto& accounts = currentUser->getAccounts();
    
    ImGui::Text("Select Account:");
    
    if (!accounts.empty()) {
        if (selectedAccount >= static_cast<int>(accounts.size())) {
            selectedAccount = 0;
        }
        
        if (ImGui::BeginCombo("##account", accounts[selectedAccount]->getAccountNumber().c_str())) {
            for (int i = 0; i < static_cast<int>(accounts.size()); i++) {
                if (ImGui::Selectable(accounts[i]->getAccountNumber().c_str(), 
                                     selectedAccount == i)) {
                    selectedAccount = i;
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::SameLine();
        ImGui::Text("Balance: %s", 
                   formatMoney(accounts[selectedAccount]->getBalance()).c_str());
    }
    
    ImGui::SameLine(650);
    if (ImGui::Button("Back", ImVec2(100, 30))) {
        currentScreen = Screen::DASHBOARD;
    }
    
    ImGui::Separator();
    ImGui::Spacing();
    
    // Transaction table
    if (!accounts.empty()) {
        auto transactions = database.getTransactions(accounts[selectedAccount]->getId());
        
        ImGui::BeginChild("TransactionList", ImVec2(0, -30), true);
        
        ImGui::Columns(5, "txns");
        ImGui::SetColumnWidth(0, 150);
        ImGui::SetColumnWidth(1, 120);
        ImGui::SetColumnWidth(2, 120);
        ImGui::SetColumnWidth(3, 120);
        ImGui::SetColumnWidth(4, 200);
        
        ImGui::Text("Date");
        ImGui::NextColumn();
        ImGui::Text("Type");
        ImGui::NextColumn();
        ImGui::Text("Amount");
        ImGui::NextColumn();
        ImGui::Text("Balance");
        ImGui::NextColumn();
        ImGui::Text("Description");
        ImGui::NextColumn();
        ImGui::Separator();
        
        for (const auto& txn : transactions) {
            ImGui::Text("%s", txn.date.c_str());
            ImGui::NextColumn();
            
            // Color based on type
            ImVec4 color;
            if (txn.type == "DEPOSIT" || txn.type == "TRANSFER_IN") {
                color = ImVec4(0.3f, 0.9f, 0.3f, 1);  // Green
            } else {
                color = ImVec4(0.9f, 0.3f, 0.3f, 1);  // Red
            }
            
            ImGui::TextColored(color, "%s", txn.type.c_str());
            ImGui::NextColumn();
            
            ImGui::TextColored(color, "%s", formatMoney(txn.amount).c_str());
            ImGui::NextColumn();
            
            ImGui::Text("%s", formatMoney(txn.balanceAfter).c_str());
            ImGui::NextColumn();
            
            ImGui::Text("%s", txn.description.c_str());
            ImGui::NextColumn();
        }
        
        ImGui::Columns(1);
        ImGui::EndChild();
        
        ImGui::Text("Total transactions: %zu", transactions.size());
    }
    
    ImGui::End();
}

//New Account screen 
void Application::showNewAccountScreen() {
    ImGui::SetNextWindowPos(ImVec2(350, 200));
    ImGui::SetNextWindowSize(ImVec2(300, 280));
    
    ImGui::Begin("Create New Account", nullptr, ImGuiWindowFlags_NoCollapse);
    
    static int accountType = 0;
    
    ImGui::Text("Account Type:");
    ImGui::RadioButton("Savings Account", &accountType, 0);
    ImGui::RadioButton("Current Account", &accountType, 1);
    
    ImGui::Spacing();
    
    if (accountType == 0) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Minimum balance: Rs. 500");
    } else {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Overdraft limit: Rs. 1000");
    }
    
    ImGui::Spacing();
    ImGui::Text("Initial Deposit:");
    ImGui::InputText("##deposit", amountInput, sizeof(amountInput));
    
    ImGui::Spacing();
    
    if (!statusMessage.empty()) {
        if (isError) {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "%s", statusMessage.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 1, 0.3f, 1), "%s", statusMessage.c_str());
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::Button("Create", ImVec2(130, 35))) {
        double deposit = atof(amountInput);
        double minDeposit = (accountType == 0) ? 500.0 : 0.0;
        
        if (deposit < minDeposit) {
            setStatus("Minimum deposit is Rs. " + std::to_string((int)minDeposit), true);
        } else {
            std::shared_ptr<Account> newAccount;
            
            if (accountType == 0) {
                auto savings = std::make_shared<SavingsAccount>(currentUser->getId());
                savings->setBalance(deposit);
                newAccount = savings;
            } else {
                auto current = std::make_shared<CurrentAccount>(currentUser->getId());
                current->setBalance(deposit);
                newAccount = current;
            }
            
            if (database.addAccount(*newAccount)) {
                if (deposit > 0) {
                    database.addTransaction(newAccount->getId(), "DEPOSIT",
                                           deposit, deposit, "Opening deposit");
                }
                
                currentUser->addAccount(newAccount);
                memset(amountInput, 0, sizeof(amountInput));
                setStatus("Account created successfully!");
            } else {
                setStatus("Failed to create account", true);
            }
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Cancel", ImVec2(130, 35))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::DASHBOARD;
    }
    
    ImGui::End();
}
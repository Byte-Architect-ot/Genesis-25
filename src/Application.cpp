#include "Application.hpp"
#include <cstring>
#include <iostream>
#include <cstdlib>

Application::Application()
    : window(sf::VideoMode(900, 600), "WiseVault - Bank System"),
      database("data/bank.db"),
      currentUser(nullptr),
      currentScreen(Screen::LOGIN),
      isError(false),
      selectedAccount(0),
      isDeposit(true) {

    window.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window)) {
        throw std::runtime_error("Failed to initialize ImGui");
    }
    ImGui::GetIO().FontGlobalScale = 1.3f;
    ImGui::StyleColorsDark();
}

Application::~Application() {
    // Clean up user pointer
    delete currentUser;
    currentUser = nullptr;
    ImGui::SFML::Shutdown();
}

void Application::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed)
                window.close();
        }

        ImGui::SFML::Update(window, clock.restart());

        switch (currentScreen) {
            case Screen::LOGIN:    showLoginScreen();    break;
            case Screen::REGISTER: showRegisterScreen(); break;
            case Screen::DASHBOARD: showDashboard();     break;
            case Screen::ACTION:   showActionScreen();   break;
        }

        window.clear(sf::Color(30, 30, 40));
        ImGui::SFML::Render(window);
        window.display();
    }
}

void Application::clearInputs() {
    memset(usernameInput, 0, sizeof(usernameInput));
    memset(passwordInput, 0, sizeof(passwordInput));
    memset(nameInput, 0, sizeof(nameInput));
    memset(amountInput, 0, sizeof(amountInput));
}

void Application::setStatus(const std::string& message, bool error) {
    statusMessage = message;
    isError = error;
}

void Application::refreshUserAccounts() {
    if (!currentUser) return;
    currentUser->clearAccounts();
    std::vector<Account*> accounts = database.getUserAccounts(currentUser->getId());
    for (int i = 0; i < (int)accounts.size(); i++) {
        currentUser->addAccount(accounts[i]);
    }
    // Don't delete here - ownership transferred to User
}

// ---- LOGIN SCREEN ----
void Application::showLoginScreen() {
    ImGui::SetNextWindowPos(ImVec2(300, 150));
    ImGui::SetNextWindowSize(ImVec2(300, 300));

    ImGui::Begin("Bank Login", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Welcome to WiseVault!");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Username:");
    ImGui::InputText("##username", usernameInput, sizeof(usernameInput));
    ImGui::Spacing();

    ImGui::Text("Password:");
    ImGui::InputText("##password", passwordInput, sizeof(passwordInput),
                     ImGuiInputTextFlags_Password);
    ImGui::Spacing();

    if (!statusMessage.empty()) {
        ImVec4 color = isError ? ImVec4(1, 0.3f, 0.3f, 1) : ImVec4(0.3f, 1, 0.3f, 1);
        ImGui::TextColored(color, "%s", statusMessage.c_str());
    }
    ImGui::Spacing();

    if (ImGui::Button("Login", ImVec2(130, 35))) {
        if (strlen(usernameInput) == 0 || strlen(passwordInput) == 0) {
            setStatus("Please enter username and password", true);
        } else {
            User* user = database.getUser(usernameInput);
            if (user && User::verifyPassword(passwordInput, user->getPassword())) {
                delete currentUser;
                currentUser = user;
                refreshUserAccounts();
                clearInputs();
                setStatus("");
                currentScreen = Screen::DASHBOARD;
            } else {
                delete user;
                setStatus("Invalid username or password", true);
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Register", ImVec2(130, 35))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::REGISTER;
    }

    ImGui::End();
}

// ---- REGISTER SCREEN ----
void Application::showRegisterScreen() {
    ImGui::SetNextWindowPos(ImVec2(250, 100));
    ImGui::SetNextWindowSize(ImVec2(400, 350));

    ImGui::Begin("Register", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Create New Account");
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

    if (!statusMessage.empty()) {
        ImVec4 color = isError ? ImVec4(1, 0.3f, 0.3f, 1) : ImVec4(0.3f, 1, 0.3f, 1);
        ImGui::TextColored(color, "%s", statusMessage.c_str());
    }
    ImGui::Spacing();

    if (ImGui::Button("Create", ImVec2(180, 35))) {
        if (strlen(nameInput) == 0 || strlen(usernameInput) == 0 || strlen(passwordInput) == 0) {
            setStatus("Please fill all fields", true);
        } else if (strlen(passwordInput) < 6) {
            setStatus("Password must be at least 6 characters", true);
        } else if (database.userExists(usernameInput)) {
            setStatus("Username already taken", true);
        } else {
            User newUser(usernameInput, passwordInput, nameInput);
            if (database.addUser(newUser)) {
                // Create a default savings account for the new user
                SavingsAccount savings(newUser.getId());
                database.addAccount(savings);
                clearInputs();
                setStatus("Account created! Please login.");
                currentScreen = Screen::LOGIN;
            } else {
                setStatus("Registration failed", true);
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Back", ImVec2(180, 35))) {
        clearInputs();
        setStatus("");
        currentScreen = Screen::LOGIN;
    }

    ImGui::End();
}

// ---- DASHBOARD SCREEN ----
void Application::showDashboard() {
    if (!currentUser) {
        currentScreen = Screen::LOGIN;
        return;
    }
    refreshUserAccounts();

    ImGui::SetNextWindowPos(ImVec2(20, 20));
    ImGui::SetNextWindowSize(ImVec2(860, 560));

    ImGui::Begin("Dashboard", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Welcome, %s!", currentUser->getFullName().c_str());
    ImGui::SameLine(700);
    if (ImGui::Button("Logout", ImVec2(100, 30))) {
        delete currentUser;
        currentUser = nullptr;
        clearInputs();
        setStatus("");
        currentScreen = Screen::LOGIN;
        ImGui::End();
        return;
    }

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Your Accounts:");
    ImGui::Spacing();

    // Simple account list
    std::vector<Account*>& accounts = currentUser->getAccounts();
    ImGui::BeginChild("AccountList", ImVec2(0, 200), true);

    ImGui::Columns(3, "accounts");
    ImGui::SetColumnWidth(0, 250);
    ImGui::SetColumnWidth(1, 200);
    ImGui::SetColumnWidth(2, 200);

    ImGui::Text("Account Number"); ImGui::NextColumn();
    ImGui::Text("Type");           ImGui::NextColumn();
    ImGui::Text("Balance");        ImGui::NextColumn();
    ImGui::Separator();

    for (int i = 0; i < (int)accounts.size(); i++) {
        ImGui::Text("%s", accounts[i]->getAccountNumber().c_str());
        ImGui::NextColumn();
        ImGui::Text("%s", accounts[i]->getAccountType().c_str());
        ImGui::NextColumn();
        ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1), "Rs. %.2f",
                          accounts[i]->getBalance());
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Text("Total Balance: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1), "Rs. %.2f",
                      currentUser->getTotalBalance());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Quick Actions:");
    ImGui::Spacing();

    if (ImGui::Button("Deposit", ImVec2(150, 45))) {
        isDeposit = true;
        clearInputs();
        setStatus("");
        currentScreen = Screen::ACTION;
    }
    ImGui::SameLine();

    if (ImGui::Button("Withdraw", ImVec2(150, 45))) {
        isDeposit = false;
        clearInputs();
        setStatus("");
        currentScreen = Screen::ACTION;
    }

    ImGui::End();
}

// ---- ACTION SCREEN (Deposit or Withdraw) ----
void Application::showActionScreen() {
    const char* title = isDeposit ? "Deposit" : "Withdraw";

    ImGui::SetNextWindowPos(ImVec2(300, 180));
    ImGui::SetNextWindowSize(ImVec2(300, 280));

    ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoCollapse);

    std::vector<Account*>& accounts = currentUser->getAccounts();

    ImGui::Text("Select Account:");
    if (!accounts.empty()) {
        if (selectedAccount >= (int)accounts.size()) selectedAccount = 0;

        std::string preview = accounts[selectedAccount]->getAccountNumber();
        if (ImGui::BeginCombo("##account", preview.c_str())) {
            for (int i = 0; i < (int)accounts.size(); i++) {
                if (ImGui::Selectable(accounts[i]->getAccountNumber().c_str(),
                                     selectedAccount == i)) {
                    selectedAccount = i;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Text("Balance: Rs. %.2f", accounts[selectedAccount]->getBalance());
    }

    ImGui::Spacing();
    ImGui::Text("Amount:");
    ImGui::InputText("##amount", amountInput, sizeof(amountInput));
    ImGui::Spacing();

    if (!statusMessage.empty()) {
        ImVec4 color = isError ? ImVec4(1, 0.3f, 0.3f, 1) : ImVec4(0.3f, 1, 0.3f, 1);
        ImGui::TextColored(color, "%s", statusMessage.c_str());
    }
    ImGui::Spacing();

    if (ImGui::Button(title, ImVec2(130, 35))) {
        double amount = atof(amountInput);
        if (amount <= 0) {
            setStatus("Enter a valid amount", true);
        } else if (!accounts.empty()) {
            Account* acc = accounts[selectedAccount];

            if (isDeposit) {
                double newBal = acc->getBalance() + amount;
                if (database.updateBalance(acc->getId(), newBal)) {
                    acc->setBalance(newBal);
                    memset(amountInput, 0, sizeof(amountInput));
                    setStatus("Deposit successful!");
                } else {
                    setStatus("Deposit failed", true);
                }
            } else {
                // Withdraw - uses polymorphic canWithdraw()
                if (!acc->canWithdraw(amount)) {
                    setStatus("Insufficient funds or limit reached", true);
                } else {
                    double newBal = acc->getBalance() - amount;
                    if (database.updateBalance(acc->getId(), newBal)) {
                        acc->setBalance(newBal);
                        memset(amountInput, 0, sizeof(amountInput));
                        setStatus("Withdrawal successful!");
                    } else {
                        setStatus("Withdrawal failed", true);
                    }
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
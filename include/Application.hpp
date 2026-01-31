#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <memory>
#include <string>
#include "User.hpp"
#include "Database.hpp"

// Different screens in our application
enum class Screen {
    LOGIN,
    REGISTER,
    DASHBOARD,
    DEPOSIT,
    WITHDRAW,
    TRANSFER,
    HISTORY,
    NEW_ACCOUNT
};

class Application {
private:
    // SFML window
    sf::RenderWindow window;
    sf::Clock clock;
    
    // Database connection
    Database database;
    
    // Currently logged in user
    std::shared_ptr<User> currentUser;
    
    // Current screen being displayed
    Screen currentScreen;
    
    // Input buffers for text fields (ImGui needs char arrays)
    char usernameInput[64] = "";
    char passwordInput[64] = "";
    char confirmPasswordInput[64] = "";
    char nameInput[128] = "";
    char amountInput[32] = "";
    char targetAccountInput[32] = "";
    
    // Status message to show user
    std::string statusMessage;
    bool isError;
    
    // Selected account index
    int selectedAccount;
    
    // Screen rendering functions
    void showLoginScreen();
    void showRegisterScreen();
    void showDashboard();
    void showDepositScreen();
    void showWithdrawScreen();
    void showTransferScreen();
    void showHistoryScreen();
    void showNewAccountScreen();
    
    // Helper functions
    void clearInputs();
    void setStatus(const std::string& message, bool error = false);
    void refreshUserAccounts();
    std::string formatMoney(double amount);

public:
    // Constructor
    Application();
    
    // Destructor
    ~Application();
    
    // Main application loop
    void run();
};

#endif
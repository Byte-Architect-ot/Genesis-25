#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <string>
#include "User.hpp"
#include "Database.hpp"

// Only 3 screens: Login, Dashboard, and Action (deposit/withdraw)
enum class Screen {
    LOGIN,
    REGISTER,
    DASHBOARD,
    ACTION
};

class Application {
private:
    sf::RenderWindow window;
    sf::Clock clock;
    Database database;

    // Raw pointer - we delete this in the destructor
    User* currentUser;

    Screen currentScreen;

    // ImGui input buffers
    char usernameInput[64] = "";
    char passwordInput[64] = "";
    char nameInput[128] = "";
    char amountInput[32] = "";

    std::string statusMessage;
    bool isError;
    int selectedAccount;
    bool isDeposit; // true = deposit mode, false = withdraw mode

    // Screen rendering
    void showLoginScreen();
    void showRegisterScreen();
    void showDashboard();
    void showActionScreen();

    // Helpers
    void clearInputs();
    void setStatus(const std::string& message, bool error = false);
    void refreshUserAccounts();

public:
    Application();
    ~Application();
    void run();
};

#endif
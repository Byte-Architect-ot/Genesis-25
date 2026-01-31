# 🏦 Simple Bank Management System

A beginner-friendly Bank Management System built with C++, SFML, and SQLite.

## ✨ Features

- ✅ User Registration & Login
- ✅ Create Bank Accounts (Savings/Current)
- ✅ Deposit Money
- ✅ Withdraw Money
- ✅ Transfer Between Accounts
- ✅ View Transaction History
- ✅ Graphical User Interface
- ✅ Data saved to SQLite Database

## 📚 Concepts Used

| Concept | Where Used |
|---------|-----------|
| Classes & Objects | User, Account, Database |
| Inheritance | SavingsAccount, CurrentAccount |
| Encapsulation | Private members with getters/setters |
| File Handling | SQLite database |
| Vectors | Storing accounts and transactions |
| Pointers | Shared pointers for memory safety |

## 🗄️ Database Structure

### Users Table
| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Unique user ID |
| username | TEXT | Login username |
| password | TEXT | Hashed password |
| name | TEXT | Full name |

### Accounts Table  
| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Unique account ID |
| user_id | INTEGER | Owner's user ID |
| account_number | TEXT | Account number |
| account_type | TEXT | SAVINGS or CURRENT |
| balance | REAL | Current balance |

### Transactions Table
| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER | Transaction ID |
| account_id | INTEGER | Related account |
| type | TEXT | DEPOSIT/WITHDRAW/TRANSFER |
| amount | REAL | Transaction amount |
| date | TEXT | Date and time |

## 🚀 How to Run

1. Install dependencies (SFML, SQLite3)
2. Build with CMake
3. Run the executable
4. Register a new account or login

## 📝 Default Test Account

After first run, you can register with:
- Username: test
- Password: Test@123
- Name: Test User
#include "Account.h"
#include <sstream>
#include <iomanip>

Account::Account(const std::string& id, const std::string& name,
                 const std::string& type, double bal)
    : accountId(id), holderName(name), accountType(type), balance(bal) {}

std::string Account::generateTxnId() const {
    std::ostringstream oss;
    oss << accountId << "-TXN-"
        << std::setw(3) << std::setfill('0') << (history.size() + 1);
    return oss.str();
}

bool Account::deposit(double amount, std::string& errorMsg) {
    if (amount <= 0.0) {
        errorMsg = "Deposit amount must be greater than zero.";
        return false;
    }
    balance += amount;
    Transaction txn(generateTxnId(), "DEPOSIT", amount, balance);
    history.push_back(txn);
    return true;
}

bool Account::withdraw(double amount, std::string& errorMsg) {
    if (amount <= 0.0) {
        errorMsg = "Withdrawal amount must be greater than zero.";
        return false;
    }
    if (amount > balance) {
        errorMsg = "Insufficient funds. Available balance: PKR " +
                   std::to_string(balance);
        return false;
    }
    balance -= amount;
    Transaction txn(generateTxnId(), "WITHDRAW", amount, balance);
    history.push_back(txn);
    return true;
}

std::string Account::toJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"accountId\":\""   << accountId   << "\","
        << "\"holderName\":\""  << holderName  << "\","
        << "\"accountType\":\"" << accountType << "\","
        << "\"balance\":"       << std::fixed << std::setprecision(2) << balance
        << "}";
    return oss.str();
}

std::string Account::historyToJson() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < history.size(); i++) {
        oss << history[i].toJson();
        if (i != history.size() - 1) oss << ",";
    }
    oss << "]";
    return oss.str();
}

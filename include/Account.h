#pragma once
#include <string>
#include <vector>
#include "Transaction.h"

class Account {
private:
    std::string              accountId;
    std::string              holderName;
    std::string              accountType;   // Savings | Current
    double                   balance;
    std::vector<Transaction> history;

public:
    Account() = default;
    Account(const std::string& id, const std::string& name,
            const std::string& type, double initialBalance = 0.0);

    std::string generateTxnId() const;

    bool deposit(double amount, std::string& errorMsg);
    bool withdraw(double amount, std::string& errorMsg);

    void addTransaction(const Transaction& txn) { history.push_back(txn); }
    void setHistory(const std::vector<Transaction>& h) { history = h; }

    std::string getAccountId()   const { return accountId;   }
    std::string getHolderName()  const { return holderName;  }
    std::string getAccountType() const { return accountType; }
    double      getBalance()     const { return balance;     }
    const std::vector<Transaction>& getHistory() const { return history; }

    std::string toJson() const;          // account summary as JSON
    std::string historyToJson() const;   // full statement as JSON array
};

#pragma once
#include <string>
#include <vector>
#include "Account.h"

class FileHandler {
public:
    static void    saveAccount(const Account& acc);
    static Account loadAccount(const std::string& id);
    static bool    accountExists(const std::string& id);
    static void    appendTransaction(const std::string& accountId, const Transaction& txn);
    static std::vector<Transaction> loadTransactions(const std::string& accountId);
    static std::vector<std::string> listAllAccountIds();
};

#include "FileHandler.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

static const std::string ACC_DIR = "data/accounts/";
static const std::string TXN_DIR = "data/transactions/";

void FileHandler::saveAccount(const Account& acc) {
    fs::create_directories(ACC_DIR);
    std::ofstream file(ACC_DIR + acc.getAccountId() + ".txt");
    if (!file.is_open())
        throw std::runtime_error("Cannot write account file.");

    file << "ID:"      << acc.getAccountId()   << "\n";
    file << "NAME:"    << acc.getHolderName()  << "\n";
    file << "TYPE:"    << acc.getAccountType() << "\n";
    file << "BALANCE:" << std::fixed << acc.getBalance() << "\n";
    file.close();
}

Account FileHandler::loadAccount(const std::string& id) {
    std::ifstream file(ACC_DIR + id + ".txt");
    if (!file.is_open())
        throw std::runtime_error("Account not found: " + id);

    std::string accId, name, type, line;
    double balance = 0.0;

    while (std::getline(file, line)) {
        if      (line.rfind("ID:",      0) == 0) accId   = line.substr(3);
        else if (line.rfind("NAME:",    0) == 0) name    = line.substr(5);
        else if (line.rfind("TYPE:",    0) == 0) type    = line.substr(5);
        else if (line.rfind("BALANCE:", 0) == 0) balance = std::stod(line.substr(8));
    }
    file.close();

    Account acc(accId, name, type, balance);
    acc.setHistory(loadTransactions(accId));
    return acc;
}

bool FileHandler::accountExists(const std::string& id) {
    return fs::exists(ACC_DIR + id + ".txt");
}

void FileHandler::appendTransaction(const std::string& accountId, const Transaction& txn) {
    fs::create_directories(TXN_DIR);
    std::ofstream file(TXN_DIR + accountId + ".txt", std::ios::app);
    if (!file.is_open())
        throw std::runtime_error("Cannot write transaction file.");
    file << txn.serialize() << "\n";
    file.close();
}

std::vector<Transaction> FileHandler::loadTransactions(const std::string& accountId) {
    std::vector<Transaction> txns;
    std::ifstream file(TXN_DIR + accountId + ".txt");
    if (!file.is_open()) return txns;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty())
            txns.push_back(Transaction::deserialize(line));
    }
    return txns;
}

std::vector<std::string> FileHandler::listAllAccountIds() {
    std::vector<std::string> ids;
    if (!fs::exists(ACC_DIR)) return ids;
    for (const auto& entry : fs::directory_iterator(ACC_DIR)) {
        if (entry.path().extension() == ".txt")
            ids.push_back(entry.path().stem().string());
    }
    return ids;
}

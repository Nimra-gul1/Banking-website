#include "Transaction.h"
#include <sstream>
#include <iomanip>
#include <ctime>

Transaction::Transaction(const std::string& id, const std::string& t,
                         double amt, double balAfter)
    : txnId(id), type(t), amount(amt), balanceAfter(balAfter),
      timestamp(std::time(nullptr)) {}

std::string Transaction::formattedTime() const {
    char buf[32];
    std::tm* tm = std::localtime(&timestamp);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", tm);
    return std::string(buf);
}

std::string Transaction::serialize() const {
    std::ostringstream oss;
    oss << txnId << "|" << type << "|"
        << std::fixed << std::setprecision(2) << amount << "|"
        << balanceAfter << "|" << timestamp;
    return oss.str();
}

Transaction Transaction::deserialize(const std::string& line) {
    std::istringstream iss(line);
    std::string id, type, amtStr, balStr, tsStr;
    std::getline(iss, id,     '|');
    std::getline(iss, type,   '|');
    std::getline(iss, amtStr, '|');
    std::getline(iss, balStr, '|');
    std::getline(iss, tsStr,  '|');

    Transaction t(id, type, std::stod(amtStr), std::stod(balStr));
    t.timestamp = (std::time_t)std::stoll(tsStr);
    return t;
}

std::string Transaction::toJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"id\":\""        << txnId       << "\","
        << "\"type\":\""      << type        << "\","
        << "\"amount\":"      << std::fixed << std::setprecision(2) << amount << ","
        << "\"balanceAfter\":"<< balanceAfter << ","
        << "\"time\":\""      << formattedTime() << "\""
        << "}";
    return oss.str();
}

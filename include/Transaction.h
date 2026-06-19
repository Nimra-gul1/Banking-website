#pragma once
#include <string>
#include <ctime>

class Transaction {
private:
    std::string txnId;
    std::string type;        // DEPOSIT | WITHDRAW
    double      amount;
    double      balanceAfter;
    std::time_t timestamp;

public:
    Transaction(const std::string& id, const std::string& type,
                double amount, double balanceAfter);

    std::string getTxnId()       const { return txnId;        }
    std::string getType()        const { return type;         }
    double      getAmount()      const { return amount;       }
    double      getBalanceAfter()const { return balanceAfter; }
    std::time_t getTimestamp()   const { return timestamp;    }

    std::string formattedTime()  const;
    std::string serialize()      const;
    std::string toJson()         const;
    static Transaction deserialize(const std::string& line);
};

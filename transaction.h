#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>

struct Transaction {
    std::string description;
    double amount;
    std::string type;
    std::string timestamp;

    Transaction(const std::string& desc, double amt, const std::string& t)
        : description(desc), amount(amt), type(t) {
        std::time_t now = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        timestamp = buf;
    }
};

#endif // TRANSACTION_H
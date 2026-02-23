#ifndef LEDGER_H
#define LEDGER_H

#include <vector>
#include <string>
#include <fstream>
#include "transaction.h"

class Ledger {
private:
    double totalCharged;
    double totalPaid;
    double balance;
    std::vector<Transaction> history;

public:
    Ledger();

    void addCharge(const std::string& desc, double amount);
    void addPayment(double amount);
    double getBalance() const;
    void printSummary() const;
    void printHistory() const;
    void applyLateFee(double percentage = 0.05);
    void saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    void exportSummary(const std::string& filename) const;

    std::string getSummary() const;
    std::string getHistory() const;

    // Getters for save/load
    double getTotalCharged() const;
    double getTotalPaid() const;
    const std::vector<Transaction>& getHistoryList() const;

    // Setters for loading
    void setTotalCharged(double val);
    void setTotalPaid(double val);
    void setBalance(double val);
    void clearAndAddHistory(const std::vector<Transaction>& newHistory);
};

#endif // LEDGER_H
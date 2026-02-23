#include "ledger.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

Ledger::Ledger() : totalCharged(0.0), totalPaid(0.0), balance(0.0) {}

void Ledger::addCharge(const std::string& desc, double amount) {
    totalCharged += amount;
    balance += amount;
    history.emplace_back(desc, amount, "Charge");
}

void Ledger::addPayment(double amount) {
    totalPaid += amount;
    balance -= amount;
    history.emplace_back("Payment", amount, "Payment");
}

double Ledger::getBalance() const {
    return balance;
}

std::string Ledger::getSummary() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Total charges: KES " << totalCharged << "<br>";
    ss << "Total payments: KES " << totalPaid << "<br>";
    ss << "Current balance: KES " << balance << "<br>";
    if (balance > 0) ss << "Owed: KES " << balance << "<br>";
    else if (balance < 0) ss << "Overpaid: KES " << -balance << "<br>";
    else ss << "Balance cleared!<br>";
    return ss.str();
}

std::string Ledger::getHistory() const {
    std::stringstream ss;
    ss << "<ul>";
    for (const auto& tx : history) {
        ss << "<li>[" << tx.timestamp << "] " << tx.type << ": " 
           << tx.description << " - KES " << std::fixed << std::setprecision(2) << tx.amount << "</li>";
    }
    ss << "</ul>";
    return ss.str();
}

void Ledger::applyLateFee(double percentage) {
    if (balance > 0) {
        double penalty = balance * percentage;
        totalCharged += penalty;
        balance += penalty;
        history.emplace_back("Late fee (" + std::to_string(percentage*100) + "%)", penalty, "Charge");
    }
}

void Ledger::exportSummary(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cout << "Error exporting file!" << std::endl;
        return;
    }
    file << "Fee Ledger Summary\n";
    file << "Total charges: KES " << std::fixed << std::setprecision(2) << totalCharged << "\n";
    file << "Total payments: KES " << totalPaid << "\n";
    file << "Current balance: KES " << balance << "\n\n";
    file << "Transaction History:\n";
    for (const auto& tx : history) {
        file << "[" << tx.timestamp << "] " << tx.type << ": "
             << tx.description << " - KES " << tx.amount << "\n";
    }
    file.close();
    std::cout << "Exported summary to " << filename << std::endl;
}
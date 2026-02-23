#include "ledger.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

Ledger::Ledger() : totalCharged(0.0), totalPaid(0.0), balance(0.0) {}

void Ledger::addCharge(const std::string& desc, double amount) {
    totalCharged += amount;
    balance += amount;
    history.emplace_back(desc, amount, "Charge");
    std::cout << "Added charge: " << desc << " - KES " 
              << std::fixed << std::setprecision(2) << amount << std::endl;
}

void Ledger::addPayment(double amount) {
    totalPaid += amount;
    balance -= amount;
    history.emplace_back("Payment", amount, "Payment");
    std::cout << "Payment recorded: KES " 
              << std::fixed << std::setprecision(2) << amount << std::endl;

    std::cout << "\n=== RECEIPT ===\n";
    std::cout << "Amount Paid: KES " << amount << "\n";
    std::cout << "New Balance: KES " << balance << "\n";
    std::cout << "===============\n";
}

double Ledger::getBalance() const {
    return balance;
}

void Ledger::printSummary() const {
    std::cout << "Total charges: KES " << std::fixed << std::setprecision(2) << totalCharged << std::endl;
    std::cout << "Total payments: KES " << totalPaid << std::endl;
    std::cout << "Current balance: KES " << balance << std::endl;
    if (balance > 0) std::cout << "Amount owed: KES " << balance << std::endl;
    else if (balance < 0) std::cout << "Overpaid by: KES " << -balance << std::endl;
    else std::cout << "Balance cleared!" << std::endl;
}

void Ledger::printHistory() const {
    std::cout << "\nTransaction History (" << history.size() << " entries):" << std::endl;
    if (history.empty()) {
        std::cout << "No transactions yet." << std::endl;
    } else {
        for (size_t i = 0; i < history.size(); ++i) {
            std::cout << (i + 1) << ". [" << history[i].timestamp << "] "
                      << history[i].type << ": "
                      << history[i].description << " - KES "
                      << std::fixed << std::setprecision(2) << history[i].amount << std::endl;
        }
    }
}

void Ledger::applyLateFee(double percentage) {
    if (balance > 0) {
        double penalty = balance * percentage;
        totalCharged += penalty;
        balance += penalty;
        history.emplace_back("Late fee penalty (" + std::to_string(percentage*100) + "%)", penalty, "Charge");
        std::cout << "Late fee applied: KES " << penalty << " (new balance: KES " << balance << ")" << std::endl;
    } else {
        std::cout << "No late fee needed (balance not overdue)." << std::endl;
    }
}

void Ledger::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cout << "Error saving file!" << std::endl;
        return;
    }
    file << totalCharged << " " << totalPaid << " " << balance << "\n";
    file << history.size() << "\n";
    for (const auto& tx : history) {
        file << tx.timestamp << "|" << tx.type << "|" << tx.description << "|" << tx.amount << "\n";
    }
    file.close();
    std::cout << "Saved ledger to " << filename << std::endl;
}

bool Ledger::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    file >> totalCharged >> totalPaid >> balance;
    size_t size;
    file >> size;
    history.clear();
    std::string line;
    std::getline(file, line); // consume newline

    for (size_t i = 0; i < size; ++i) {
        std::getline(file, line);
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);
        size_t pos3 = line.find('|', pos2 + 1);

        std::string timestamp = line.substr(0, pos1);
        std::string type = line.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string desc = line.substr(pos2 + 1, pos3 - pos2 - 1);
        double amt = std::stod(line.substr(pos3 + 1));

        Transaction tx(desc, amt, type);
        tx.timestamp = timestamp;
        history.push_back(tx);
    }
    file.close();
    std::cout << "Loaded ledger from " << filename << std::endl;
    return true;
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

// Setters for loading
double Ledger::getTotalCharged() const { return totalCharged; }
double Ledger::getTotalPaid() const { return totalPaid; }
const std::vector<Transaction>& Ledger::getHistoryList() const { return history; }

void Ledger::setTotalCharged(double val) { totalCharged = val; }
void Ledger::setTotalPaid(double val) { totalPaid = val; }
void Ledger::setBalance(double val) { balance = val; }
void Ledger::clearAndAddHistory(const std::vector<Transaction>& newHistory) {
    history = newHistory;
}


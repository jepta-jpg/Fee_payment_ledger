#include "student.h"

Student::Student(const std::string& n, const std::string& i, const std::string& c)
    : name(n), id(i), className(c) {}

const std::string& Student::getName() const { return name; }
const std::string& Student::getId() const { return id; }
const std::string& Student::getClassName() const { return className; }
const Ledger& Student::getLedger() const { return ledger; }
Ledger& Student::getLedgerMutable() { return ledger; }
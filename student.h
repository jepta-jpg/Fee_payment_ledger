#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include "ledger.h"

class Student {
private:
    std::string name;
    std::string id;
    std::string className;
    Ledger ledger;

public:
    Student(const std::string& n = "", const std::string& i = "", const std::string& c = "");

    const std::string& getName() const;
    const std::string& getId() const;
    const std::string& getClassName() const;
    const Ledger& getLedger() const;
    Ledger& getLedgerMutable();
};

#endif // STUDENT_H
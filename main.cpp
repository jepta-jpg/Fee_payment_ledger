#include "httplib.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include "ledger.h"
#include "student.h"
#include "transaction.h"

using namespace httplib;
using namespace std;

vector<Student> students;
int currentStudentIndex = -1;
string loggedInUser = "";

// Load data from students.json on startup
void loadData() {
    ifstream file("students.json");
    if (!file.is_open()) {
        cout << "No saved data found" << endl;
        return;
    }

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    size_t pos = 0;
    students.clear();
    while ((pos = content.find("{\"name\":\"", pos)) != string::npos) {
        pos += 9;
        size_t end = content.find("\"", pos);
        string name = content.substr(pos, end - pos);
        pos = end + 1;

        pos = content.find("\"id\":\"", pos) + 6;
        end = content.find("\"", pos);
        string id = content.substr(pos, end - pos);
        pos = end + 1;

        pos = content.find("\"className\":\"", pos) + 13;
        end = content.find("\"", pos);
        string className = content.substr(pos, end - pos);
        pos = end + 1;

        students.emplace_back(name, id, className);
        auto& ledger = students.back().getLedgerMutable();

        pos = content.find("\"totalCharged\":", pos) + 16;
        end = content.find(",", pos);
        ledger.setTotalCharged(stod(content.substr(pos, end - pos)));
        pos = end + 1;

        pos = content.find("\"totalPaid\":", pos) + 12;
        end = content.find(",", pos);
        ledger.setTotalPaid(stod(content.substr(pos, end - pos)));
        pos = end + 1;

        pos = content.find("\"balance\":", pos) + 10;
        end = content.find(",", pos);
        ledger.setBalance(stod(content.substr(pos, end - pos)));
        pos = end + 1;

        pos = content.find("\"history\": [", pos) + 12;
        vector<Transaction> hist;
        while (pos < content.find("]", pos)) {
            pos = content.find("{\"description\":\"", pos) + 16;
            end = content.find("\"", pos);
            string desc = content.substr(pos, end - pos);
            pos = end + 1;

            pos = content.find("\"amount\":", pos) + 9;
            end = content.find(",", pos);
            double amt = stod(content.substr(pos, end - pos));
            pos = end + 1;

            pos = content.find("\"type\":\"", pos) + 8;
            end = content.find("\"", pos);
            string type = content.substr(pos, end - pos);
            pos = end + 1;

            pos = content.find("\"timestamp\":\"", pos) + 13;
            end = content.find("\"", pos);
            string timestamp = content.substr(pos, end - pos);
            pos = end + 1;

            Transaction tx(desc, amt, type);
            tx.timestamp = timestamp;
            hist.push_back(tx);
        }
        ledger.clearAndAddHistory(hist);
        pos = content.find("}", pos);
    }
    if (!students.empty()) currentStudentIndex = 0;
    cout << "Loaded " << students.size() << " students from students.json" << endl;
}

// Save data to students.json
void saveData() {
    ofstream file("students.json");
    if (!file.is_open()) {
        cout << "Error saving students.json" << endl;
        return;
    }

    file << "[\n";
    for (size_t i = 0; i < students.size(); ++i) {
        const auto& s = students[i];
        file << "  {\n";
        file << "    \"name\": \"" << s.getName() << "\",\n";
        file << "    \"id\": \"" << s.getId() << "\",\n";
        file << "    \"className\": \"" << s.getClassName() << "\",\n";
        file << "    \"ledger\": {\n";
        file << "      \"totalCharged\": " << s.getLedger().getTotalCharged() << ",\n";
        file << "      \"totalPaid\": " << s.getLedger().getTotalPaid() << ",\n";
        file << "      \"balance\": " << s.getLedger().getBalance() << ",\n";
        file << "      \"history\": [\n";
        const auto& hist = s.getLedger().getHistoryList();
        for (size_t j = 0; j < hist.size(); ++j) {
            const auto& tx = hist[j];
            file << "        {\"description\": \"" << tx.description << "\", \"amount\": " << tx.amount 
                 << ", \"type\": \"" << tx.type << "\", \"timestamp\": \"" << tx.timestamp << "\"}";
            if (j < hist.size() - 1) file << ",";
            file << "\n";
        }
        file << "      ]\n";
        file << "    }\n";
        file << "  }";
        if (i < students.size() - 1) file << ",";
        file << "\n";
    }
    file << "]";
    file.close();
    cout << "Saved " << students.size() << " students to students.json" << endl;
}

// Beautiful HTML/CSS (Apple-style, dark mode, interactive)
string header = R"(
<!DOCTYPE html>
<html lang="en" class="scroll-smooth" data-theme="light">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Fee Ledger</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/css/all.min.css">
  <script src="https://cdn.tailwindcss.com"></script>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    :root {
      --bg: linear-gradient(135deg, #e0eafc, #cfdef3);
      --text: #1d1d1f;
      --card-bg: rgba(255, 255, 255, 0.8);
      --card-border: rgba(0, 0, 0, 0.08);
      --primary: #6366f1;
      --primary-dark: #4f46e5;
      --shadow: 0 10px 40px rgba(0,0,0,0.08);
      --accent: #8b5cf6;
    }
    [data-theme="dark"] {
      --bg: linear-gradient(135deg, #0f0f11, #1c1c1e);
      --text: #f5f5f7;
      --card-bg: rgba(28, 28, 30, 0.8);
      --card-border: rgba(255, 255, 255, 0.1);
      --shadow: 0 10px 40px rgba(0,0,0,0.4);
    }
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background: var(--bg);
      color: var(--text);
      min-height: 100vh;
      transition: background 0.5s ease, color 0.5s ease;
    }
    .container { max-width: 1280px; margin: 0 auto; padding: 0 24px; }
    h1 { font-size: 3.75rem; font-weight: 700; letter-spacing: -0.025em; line-height: 1.1; }
    .card {
      background: var(--card-bg);
      border-radius: 24px;
      backdrop-filter: blur(20px);
      border: 1px solid var(--card-border);
      box-shadow: var(--shadow);
      padding: 32px;
      margin-bottom: 32px;
      transition: all 0.4s cubic-bezier(0.165, 0.84, 0.44, 1);
    }
    .card:hover {
      transform: translateY(-8px);
      box-shadow: 0 20px 60px rgba(0,0,0,0.15);
    }
    .btn {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      padding: 14px 32px;
      border-radius: 9999px;
      font-weight: 600;
      font-size: 1.125rem;
      transition: all 0.3s ease;
      backdrop-filter: blur(10px);
      box-shadow: 0 4px 12px rgba(0,0,0,0.1);
    }
    .btn-primary { background: var(--primary); color: white; }
    .btn-primary:hover { background: var(--primary-dark); transform: translateY(-2px); }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(340px, 1fr)); gap: 2rem; }
    .student-card { text-align: center; transition: all 0.4s ease; }
    .student-card:hover { transform: scale(1.04); }
    .balance-positive { color: #ef4444; font-weight: 700; font-size: 2.5rem; }
    .balance-negative { color: #10b981; font-weight: 700; font-size: 2.5rem; }
    .balance-zero { color: #10b981; font-weight: 700; font-size: 2.5rem; }
    .theme-toggle {
      cursor: pointer;
      padding: 12px;
      border-radius: 50%;
      background: rgba(0,0,0,0.06);
      transition: all 0.3s;
    }
    .dark .theme-toggle { background: rgba(255,255,255,0.1); }
  </style>
</head>
<body>
  <div class="container py-16">
    <div class="flex justify-between items-center mb-16">
      <h1 class="text-6xl font-bold text-primary">Fee Ledger</h1>
      <button id="themeToggle" class="theme-toggle text-2xl">
        <i class="fas fa-moon dark:hidden"></i>
        <i class="fas fa-sun hidden dark:inline"></i>
      </button>
    </div>
)";

string footer = R"(
  </div>

  <script>
    const toggle = document.getElementById('themeToggle');
    toggle?.addEventListener('click', () => {
      document.documentElement.classList.toggle('dark');
      localStorage.setItem('theme', document.documentElement.classList.contains('dark') ? 'dark' : 'light');
    });
    if (localStorage.getItem('theme') === 'dark') {
      document.documentElement.classList.add('dark');
    }
  </script>
</body>
</html>
)";

// Routes
void setupRoutes() {
    Server svr;

    svr.Get("/", [](const Request&, Response &res) {
        string body = header + "<h1>Fee Ledger - Students</h1><ul>";
        if (students.empty()) {
            body += "<li>No students yet. <a href=\"/add-student\">Add one</a></li>";
        } else {
            for (size_t i = 0; i < students.size(); ++i) {
                body += "<li><a href=\"/student/" + to_string(i) + "\">" 
                        + students[i].getName() + " (" + students[i].getId() + ")</a></li>";
            }
        }
        body += "</ul><br><a href=\"/add-student\">Add New Student</a>";
        body += footer;
        res.set_content(body, "text/html");
    });

    svr.Get("/add-student", [](const Request&, Response &res) {
        string body = header + "<h1>Add New Student</h1>"
            "<form method=\"POST\" action=\"/add-student\">"
            "Name: <input type=\"text\" name=\"name\" required><br><br>"
            "ID: <input type=\"text\" name=\"id\" required><br><br>"
            "Class/Grade: <input type=\"text\" name=\"className\" required><br><br>"
            "<input type=\"submit\" value=\"Add Student\">"
            "</form><br><a href=\"/\">Back</a>" + footer;
        res.set_content(body, "text/html");
    });

    svr.Post("/add-student", [](const Request& req, Response &res) {
        string name = req.get_param_value("name");
        string id = req.get_param_value("id");
        string className = req.get_param_value("className");

        students.emplace_back(name, id, className);
        currentStudentIndex = students.size() - 1;

        saveData();

        res.set_redirect("/");
    });

    svr.Get(R"(/student/(\d+))", [](const Request& req, Response &res) {
        int idx = stoi(req.matches[1]);
        if (idx < 0 || idx >= students.size()) {
            res.set_content("Student not found", "text/plain");
            return;
        }
        currentStudentIndex = idx;
        const auto& stu = students[idx];

        string body = header + "<h1>" + stu.getName() + " (" + stu.getId() + " - " + stu.getClassName() + ")</h1>";
        body += "<p>" + stu.getLedger().getSummary() + "</p>";
        body += "<h2>Transaction History</h2>" + stu.getLedger().getHistory();

        body += "<br><a href=\"/add-charge\">Add Charge</a> | ";
        body += "<a href=\"/add-payment\">Add Payment</a> | ";
        body += "<a href=\"/apply-late-fee\">Apply Late Fee</a> | ";
        body += "<a href=\"/export\">Export Summary</a> | ";
        body += "<a href=\"/\">Back to Students</a>";

        body += footer;
        res.set_content(body, "text/html");
    });

    svr.Get("/add-charge", [](const Request&, Response &res) {
        string body = header + "<h1>Add Charge</h1>"
            "<form method=\"POST\" action=\"/add-charge\">"
            "Description: <input type=\"text\" name=\"desc\" required><br><br>"
            "Amount (KES): <input type=\"number\" step=\"0.01\" name=\"amount\" required><br><br>"
            "<input type=\"submit\" value=\"Add Charge\">"
            "</form><br><a href=\"/student/" + to_string(currentStudentIndex) + "\">Back</a>" + footer;
        res.set_content(body, "text/html");
    });

    svr.Post("/add-charge", [](const Request& req, Response &res) {
        double amount = stod(req.get_param_value("amount"));
        string desc = req.get_param_value("desc");
        students[currentStudentIndex].getLedgerMutable().addCharge(desc, amount);
        saveData();
        res.set_redirect("/student/" + to_string(currentStudentIndex));
    });

    svr.Get("/add-payment", [](const Request&, Response &res) {
        string body = header + "<h1>Record Payment</h1>"
            "<form method=\"POST\" action=\"/add-payment\">"
            "Amount (KES): <input type=\"number\" step=\"0.01\" name=\"amount\" required><br><br>"
            "<input type=\"submit\" value=\"Record Payment\">"
            "</form><br><a href=\"/student/" + to_string(currentStudentIndex) + "\">Back</a>" + footer;
        res.set_content(body, "text/html");
    });

    svr.Post("/add-payment", [](const Request& req, Response &res) {
        double amount = stod(req.get_param_value("amount"));
        students[currentStudentIndex].getLedgerMutable().addPayment(amount);
        saveData();
        res.set_redirect("/student/" + to_string(currentStudentIndex));
    });

    svr.Get("/apply-late-fee", [](const Request&, Response &res) {
        students[currentStudentIndex].getLedgerMutable().applyLateFee();
        saveData();
        res.set_redirect("/student/" + to_string(currentStudentIndex));
    });

    svr.Get("/export", [](const Request&, Response &res) {
        string filename = "summary_" + students[currentStudentIndex].getId() + ".txt";
        students[currentStudentIndex].getLedger().exportSummary(filename);
        res.set_content("Summary exported to " + filename + "<br><a href=\"/student/" + to_string(currentStudentIndex) + "\">Back</a>", "text/html");
    });

    cout << "Web server starting at http://localhost:8080" << endl;
    svr.listen("0.0.0.0", 8080);
}

int main() {
    loadData();
    setupRoutes();
    return 0;
}
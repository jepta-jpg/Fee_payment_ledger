#include "httplib.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "ledger.h"
#include "student.h"
#include "transaction.h"

using namespace httplib;
using namespace std;

vector<Student> students;
int currentStudentIndex = -1;

string header = R"(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Fee Ledger</title>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/css/all.min.css">
<script src="https://cdn.tailwindcss.com"></script>
</head>
<body class="bg-gradient-to-br from-indigo-50 via-white to-blue-50 min-h-screen">
<div class="container mx-auto px-4 py-8">
)";

string footer = "</div></body></html>";

void setupRoutes() {
    Server svr;

    svr.Get("/", [](const Request&, Response &res) {
        string body = header + R"(
<header class="text-center mb-12">
  <h1 class="text-5xl font-bold text-indigo-700 mb-4">Fee Ledger</h1>
  <p class="text-xl text-gray-600">Manage student fees easily</p>
</header>

<div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
)";

        if (students.empty()) {
            body += R"(
<div class="col-span-full text-center py-16 bg-white rounded-2xl shadow-lg border border-gray-100">
  <i class="fas fa-users text-6xl text-indigo-300 mb-6"></i>
  <h2 class="text-2xl font-semibold text-gray-700 mb-4">No students yet</h2>
  <a href="/add-student" class="inline-block bg-indigo-600 text-white px-8 py-4 rounded-xl font-medium hover:bg-indigo-700 transition">
    Add First Student
  </a>
</div>
)";
        } else {
            for (size_t i = 0; i < students.size(); ++i) {
                const auto& s = students[i];
                double bal = s.getLedger().getBalance();
                string balClass = bal > 0 ? "text-red-600" : (bal < 0 ? "text-green-600" : "text-gray-600");
                string balText = bal > 0 ? "Owed" : (bal < 0 ? "Overpaid" : "Cleared");

                body += R"(
<div class="bg-white rounded-2xl shadow-lg hover:shadow-xl transition-all duration-300 overflow-hidden border border-gray-100">
  <div class="p-6">
    <h3 class="text-xl font-bold text-indigo-700 mb-2">)" + s.getName() + R"(</h3>
    <p class="text-gray-600 mb-4">ID: )" + s.getId() + R"( • Class: )" + s.getClassName() + R"(</p>
    <p class="text-lg font-semibold )" + balClass + R"( mb-1">KES )" + to_string(bal) + R"(</p>
    <p class="text-sm text-gray-500 mt-1">)" + balText + R"(</p>
  </div>
  <div class="bg-gray-50 px-6 py-4 border-t border-gray-100">
    <a href="/student/)" + to_string(i) + R"(" class="text-indigo-600 hover:text-indigo-800 font-medium flex items-center">
      View Ledger <i class="fas fa-arrow-right ml-2"></i>
    </a>
  </div>
</div>
)";
            }
        }

        body += R"(
</div>

<div class="text-center mt-12">
  <a href="/add-student" class="inline-block bg-indigo-600 text-white px-10 py-5 rounded-full font-semibold text-lg hover:bg-indigo-700 transition shadow-lg">
    Add New Student
  </a>
</div>
)" + footer;

        res.set_content(body, "text/html");
    });

    // Add-student form
    svr.Get("/add-student", [](const Request&, Response &res) {
        string body = header + R"(
<div class="max-w-lg mx-auto mt-12">
  <div class="bg-white rounded-2xl shadow-xl p-8">
    <h1 class="text-3xl font-bold text-indigo-700 mb-8 text-center">Add New Student</h1>
    <form method="POST" action="/add-student" class="space-y-6">
      <div>
        <label class="block text-gray-700 font-medium mb-2">Full Name</label>
        <input type="text" name="name" required class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500">
      </div>
      <div>
        <label class="block text-gray-700 font-medium mb-2">Student ID</label>
        <input type="text" name="id" required class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500">
      </div>
      <div>
        <label class="block text-gray-700 font-medium mb-2">Class/Grade</label>
        <input type="text" name="className" required class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500">
      </div>
      <button type="submit" class="w-full bg-indigo-600 text-white py-4 rounded-xl font-semibold hover:bg-indigo-700 transition">
        Add Student
      </button>
    </form>
  </div>
  <div class="text-center mt-8">
    <a href="/" class="text-indigo-600 hover:text-indigo-800 font-medium">Back to Dashboard</a>
  </div>
</div>
)" + footer;
        res.set_content(body, "text/html");
    });

    svr.Post("/add-student", [](const Request& req, Response &res) {
        string name = req.get_param_value("name");
        string id = req.get_param_value("id");
        string className = req.get_param_value("className");

        students.emplace_back(name, id, className);
        currentStudentIndex = students.size() - 1;

        res.set_redirect("/");
    });

    // Student detail page
    svr.Get(R"(/student/(\d+))", [](const Request& req, Response &res) {
        int idx = stoi(req.matches[1]);
        if (idx < 0 || idx >= students.size()) {
            res.set_content("Student not found", "text/plain");
            return;
        }
        currentStudentIndex = idx;
        const auto& stu = students[idx];
        double bal = stu.getLedger().getBalance();
        string balClass = bal > 0 ? "text-red-600" : (bal < 0 ? "text-green-600" : "text-gray-800");

        string body = header + R"(
<div class="container mx-auto px-4 py-12">
  <div class="bg-white rounded-2xl shadow-xl p-8 mb-8">
    <div class="flex justify-between items-center mb-8">
      <h1 class="text-4xl font-bold text-indigo-700">)" + stu.getName() + R"(</h1>
      <span class="px-6 py-3 rounded-full text-lg font-semibold )" + (bal > 0 ? "bg-red-100 text-red-800" : (bal < 0 ? "bg-green-100 text-green-800" : "bg-gray-100 text-gray-800")) + R"(">
        Balance: KES )" + to_string(bal) + R"(
      </span>
    </div>
    <p class="text-gray-600 mb-2">ID: )" + stu.getId() + R"(</p>
    <p class="text-gray-600 mb-8">Class: )" + stu.getClassName() + R"(</p>

    <div class="grid grid-cols-1 lg:grid-cols-2 gap-8">
      <div class="bg-white rounded-xl shadow-lg p-6">
        <h2 class="text-2xl font-bold mb-6">Summary</h2>
        <div class="prose max-w-none">
          )" + stu.getLedger().getSummary() + R"(
        </div>
      </div>

      <div class="bg-white rounded-xl shadow-lg p-6">
        <h2 class="text-2xl font-bold mb-6">Balance Over Time</h2>
        <canvas id="balanceChart" height="200"></canvas>
      </div>
    </div>
  </div>

  <div class="bg-white rounded-2xl shadow-xl p-8 mb-8">
    <h2 class="text-2xl font-bold mb-6">Transaction History</h2>
    )" + stu.getLedger().getHistory() + R"(
  </div>

  <div class="grid grid-cols-2 md:grid-cols-4 gap-4">
    <a href="/add-charge" class="btn bg-indigo-600 hover:bg-indigo-700 text-white py-4 rounded-xl text-center font-medium transition">
      <i class="fas fa-plus mr-2"></i> Add Charge
    </a>
    <a href="/add-payment" class="btn bg-green-600 hover:bg-green-700 text-white py-4 rounded-xl text-center font-medium transition">
      <i class="fas fa-money-bill-wave mr-2"></i> Record Payment
    </a>
    <a href="/apply-late-fee" class="btn bg-amber-500 hover:bg-amber-600 text-white py-4 rounded-xl text-center font-medium transition">
      <i class="fas fa-exclamation-triangle mr-2"></i> Apply Late Fee
    </a>
    <a href="/export" class="btn bg-blue-600 hover:bg-blue-700 text-white py-4 rounded-xl text-center font-medium transition">
      <i class="fas fa-file-export mr-2"></i> Export
    </a>
  </div>

  <div class="text-center mt-10">
    <a href="/" class="text-indigo-600 hover:text-indigo-800 font-medium">
      <i class="fas fa-arrow-left mr-2"></i> Back to Dashboard
    </a>
  </div>
</div>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script>
  const ctx = document.getElementById('balanceChart')?.getContext('2d');
  if (ctx) {
    new Chart(ctx, {
      type: 'line',
      data: {
        labels: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun'],
        datasets: [{
          label: 'Balance',
          data: [12000, 8000, 15000, 5000, -2000, 3000],
          borderColor: '#4361ee',
          backgroundColor: 'rgba(67, 97, 238, 0.2)',
          tension: 0.4,
          fill: true
        }]
      },
      options: {
        responsive: true,
        scales: { y: { beginAtZero: false } }
      }
    });
  }
</script>
)" + footer;

        res.set_content(body, "text/html");
    });

    // Add charge route
    svr.Get("/add-charge", [](const Request&, Response &res) {
        string body = header + R"(
<div class="container mx-auto px-4 py-12 max-w-lg">
  <div class="bg-white rounded-2xl shadow-xl p-8">
    <h1 class="text-3xl font-bold text-indigo-700 mb-8 text-center">Add Charge</h1>
    <form method="POST" action="/add-charge" class="space-y-6">
      <div>
        <label class="block text-gray-700 font-medium mb-2">Description</label>
        <input type="text" name="desc" required class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500">
      </div>
      <div>
        <label class="block text-gray-700 font-medium mb-2">Amount (KES)</label>
        <input type="number" step="0.01" name="amount" required class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500">
      </div>
      <button type="submit" class="w-full bg-indigo-600 text-white py-4 rounded-xl font-semibold hover:bg-indigo-700 transition">
        Add Charge
      </button>
    </form>
  </div>
  <div class="text-center mt-8">
    <a href="/student/" + to_string(currentStudentIndex) + R"(" class="text-indigo-600 hover:text-indigo-800 font-medium">Back to Ledger</a>
  </div>
</div>
)" + footer;
        res.set_content(body, "text/html");
    });

    svr.Post("/add-charge", [](const Request& req, Response &res) {
        double amount = stod(req.get_param_value("amount"));
        string desc = req.get_param_value("desc");
        students[currentStudentIndex].getLedgerMutable().addCharge(desc, amount);
        res.set_redirect("/student/" + to_string(currentStudentIndex));
    });

    // Add payment route
    svr.Get("/add-payment", [](const Request&, Response &res) {
        string body = header + R"(
<div class="container mx-auto px-4 py-12 max-w-lg">
  <div class="bg-white rounded-2xl shadow-xl p-8">
    <h1 class="text-3xl font-bold text-indigo-700 mb-8 text-center">Record Payment</h1>
    <form method="POST" action="/add-payment" class="space-y-6">
      <div>
        <label class="block text-gray-700 font-medium mb-2">Amount (KES)</label>
        <input type="number" step="0.01" name="amount" required class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500">
      </div>
      <button type="submit" class="w-full bg-green-600 text-white py-4 rounded-xl font-semibold hover:bg-green-700 transition">
        Record Payment
      </button>
    </form>
  </div>
  <div class="text-center mt-8">
    <a href="/student/" + to_string(currentStudentIndex) + R"(" class="text-indigo-600 hover:text-indigo-800 font-medium">Back to Ledger</a>
  </div>
</div>
)" + footer;
        res.set_content(body, "text/html");
    });

    svr.Post("/add-payment", [](const Request& req, Response &res) {
        double amount = stod(req.get_param_value("amount"));
        students[currentStudentIndex].getLedgerMutable().addPayment(amount);
        res.set_redirect("/student/" + to_string(currentStudentIndex));
    });

    svr.Get("/apply-late-fee", [](const Request&, Response &res) {
        students[currentStudentIndex].getLedgerMutable().applyLateFee();
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
    setupRoutes();
    return 0;
}
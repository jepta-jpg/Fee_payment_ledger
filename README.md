# 📑 Fee Ledger Web App

> **A sleek, modern, and high-performance fee management system.**

---

## 🚀 Overview
A simple, modern, and beautiful browser-based fee management system for schools, institutions, or personal use. Track student balances, record charges/payments, and export summaries—all through a clean, responsive web interface.

### 🛠️ Built With
* **Backend:** C++ (Logic) & `cpp-httplib` (Server)
* **Frontend:** HTML5 & Tailwind CSS (UI)
* **Icons:** Font Awesome
* **Database:** Persistent JSON Storage (`students.json`)

---

## ✨ Key Features

* **User Dashboard:** High-level stats and quick action shortcuts.
* **Transaction History:** Full logs with timestamps for every charge and payment.
* **Late Fees:** Apply an automatic 5% penalty to outstanding balances.
* **Dark Mode:** Seamless transition between light and dark UI (saved in-browser).
* **Data Persistence:** Your data survives server restarts.

---

## 📥 Installation & Setup

### 1. Requirements
* **Windows** 10/11
* **MinGW-w64** (g++ compiler)

### 2. Compilation
```bash
g++ main.cpp ledger.cpp student.cpp transaction.cpp -o feeledger.exe -mconsole -std=c++17 -pthread -lws2_32

Execution
./feeledger.exe

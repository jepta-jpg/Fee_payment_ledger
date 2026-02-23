Fee Ledger Web App
A simple, modern, browser-based fee management system for schools or institutions.
Track student balances, add charges/payments, apply late fees, view transaction history with timestamps, and export summaries — all through a clean web interface.
Built with:

C++ (backend)
cpp-httplib (lightweight HTTP server)
HTML/CSS (Tailwind + custom styles for Apple-inspired minimal design)
Persistent JSON storage (students.json)

Features

Web interface (no installation needed — just open in browser)
Add / list / view students
Record fee charges and payments (with console receipt)
Apply automatic late fee (5% default)
View transaction history with dates
Export student ledger summary to text file
Data persistence (saved automatically to students.json — survives server restarts)
Beautiful, responsive, modern UI with dark mode toggle
Simple login screen (demo credentials: admin / 1234)

Screenshots
Failed to load imageView link
Dashboard with student cards (light mode)
Failed to load imageView link
Student ledger with summary, history, and actions
Failed to load imageView link
Dark mode toggle active
Requirements

Windows (tested on MinGW-w64)
g++ compiler (MinGW)
Internet connection (for Tailwind CDN & Chart.js on first load — optional after that)

Installation & Setup

Clone or download this repository
Install MinGW-w64 (if not already installed)
Download from: https://www.mingw-w64.org/downloads/
Or use MSYS2: https://www.msys2.org/
Make sure g++ is in your PATH

Place all files in one folder (e.g. C:\Users\YourName\Desktop\FeeLedger)Required files:
main.cpp
ledger.h & ledger.cpp
student.h & student.cpp
transaction.h & transaction.cpp
httplib.h (download from https://github.com/yhirose/cpp-httplib/raw/v0.12.0/httplib.h)

Open Command Prompt in the project foldertextcd C:\Users\YourName\Desktop\FeeLedger
Compile the applicationtextg++ main.cpp ledger.cpp student.cpp transaction.cpp -o feeledger.exe -mconsole -std=c++17 -pthread -lws2_32
Run the servertextfeeledger.exeYou should see:textWeb server starting at http://localhost:8080
Open in browserGo to: http://localhost:8080 or http://127.0.0.1:8080

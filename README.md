# Fee Ledger Web App

Modern browser-based application for managing school fees / student ledgers.

## Features

- Add students (name, ID, class/grade)
- Record charges & payments
- Apply late fee (default 5%)
- View transaction history with timestamps
- Export ledger summary to text file
- Persistent data (saved to `students.json` — survives server restarts)
- Responsive web interface
- Dark mode toggle (saved in browser)
- Clean dashboard with quick actions

## Tech Stack

- **Backend**: C++17
- **Web server**: cpp-httplib (single header)
- **Frontend**: HTML + Tailwind CSS (via CDN) + Font Awesome icons
- **Chart**: Chart.js (via CDN)
- **Persistence**: JSON file (`students.json`)

## Requirements

- Windows
- MinGW-w64 (with g++)
- Internet connection (first run only — for CDNs)

## Setup

1. Place all files in one folder

   Required files:
   - `main.cpp`
   - `ledger.h` + `ledger.cpp`
   - `student.h` + `student.cpp`
   - `transaction.h` + `transaction.cpp`
   - `httplib.h` (download: https://raw.githubusercontent.com/yhirose/cpp-httplib/v0.12.0/httplib.h)

2. Open Command Prompt in the folder

   ```bash
   cd path\to\FeeLedger


## Compile

g++ main.cpp ledger.cpp student.cpp transaction.cpp -o feeledger.exe -mconsole -std=c++17 -pthread -lws2_32

## Run

feeledger.exe

## Open in browser 
http://localhost:8080

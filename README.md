# MDBest Banking System — Web Edition

A real C++ backend (HTTP server) powering a browser-based banking console.
Every button click in the browser sends a request to the C++ server, which
validates it, updates account state, and writes to disk — same logic as the
CLI version, now reachable from a web page.

## How it works

```
Browser (public/index.html)
        │  fetch() calls
        ▼
C++ HTTP Server (banking_server.exe)
        │  Account / Transaction / FileHandler classes
        ▼
data/accounts/*.txt   data/transactions/*.txt
```

- The C++ server uses [cpp-httplib](https://github.com/yhirose/cpp-httplib)
  (a single header file, already included — no install needed).
- All banking logic (validation, balances, transaction history) runs in C++.
- The frontend is plain HTML/CSS/JS — no frameworks, no build step.

## Folder structure

```
banking_web/
├── include/
│   ├── httplib.h        ← HTTP server library (single header)
│   ├── Account.h
│   ├── Transaction.h
│   └── FileHandler.h
├── src/
│   ├── main.cpp          ← server + REST API routes
│   ├── Account.cpp
│   ├── Transaction.cpp
│   └── FileHandler.cpp
├── public/
│   └── index.html        ← the website (served by the C++ server itself)
├── data/                  ← created automatically when you run it
└── Makefile
```

## How to run it (Windows, in VS Code or Command Prompt / PowerShell)

You already have `g++` installed from before. Just run:

```powershell
cd banking_web
g++ -std=c++17 -I include -O2 -o banking_server.exe src/main.cpp src/Account.cpp src/Transaction.cpp src/FileHandler.cpp -lws2_32
.\banking_server.exe
```

You'll see:

```
  ============================================
   MDBest Banking System — Web Server
   Running at: http://localhost:8080
   Press Ctrl+C to stop
  ============================================
```

Now open your browser and go to:

**http://localhost:8080**

That's it — the website is being served directly by your C++ program.
Create an account, deposit, withdraw, and view statements, all through the browser.
Leave the terminal window open while you use the site; closing it stops the server.

## How to run it (Linux / Mac)

```bash
cd banking_web
make
./banking_server
```

## API Reference (for reference / if you want to extend it)

| Method | Endpoint                              | Description              |
|--------|----------------------------------------|---------------------------|
| POST   | `/api/accounts`                        | Create a new account      |
| GET    | `/api/accounts`                        | List all accounts         |
| GET    | `/api/accounts/:id`                    | Get one account's summary |
| GET    | `/api/accounts/:id/statement`          | Get full transaction history |
| POST   | `/api/accounts/:id/deposit`            | Deposit funds              |
| POST   | `/api/accounts/:id/withdraw`           | Withdraw funds              |

All POST bodies are JSON, e.g. `{"amount": "5000"}`.

## Notes

- Data persists in the `data/` folder as plain text files — same format as
  the CLI version. Stopping and restarting the server keeps your accounts.
- `httplib.h` is MIT licensed and bundled directly in `include/` so there's
  nothing extra to install.

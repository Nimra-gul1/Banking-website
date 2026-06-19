#include "httplib.h"
#include "Account.h"
#include "FileHandler.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>

// ─────────────────────────────────────────────────────────────────────────
// Tiny helper: extract a string value for "key" from a flat JSON body
// (Good enough for our simple, known request shapes — no external JSON lib)
// ─────────────────────────────────────────────────────────────────────────
static std::string extractJsonValue(const std::string& body, const std::string& key) {
    std::string pattern = "\"" + key + "\"";
    size_t pos = body.find(pattern);
    if (pos == std::string::npos) return "";
    pos = body.find(':', pos);
    if (pos == std::string::npos) return "";
    pos++;
    while (pos < body.size() && (body[pos] == ' ' || body[pos] == '\"')) pos++;
    size_t end = pos;
    while (end < body.size() && body[end] != ',' && body[end] != '}' && body[end] != '\"') end++;
    std::string val = body.substr(pos, end - pos);
    return val;
}

static std::string generateAccountId() {
    std::time_t now = std::time(nullptr);
    std::ostringstream oss;
    oss << "ACC-" << now;
    return oss.str();
}

static std::string jsonError(const std::string& msg) {
    return "{\"success\":false,\"error\":\"" + msg + "\"}";
}

int main() {
    httplib::Server svr;

    // ── Serve the frontend (index.html, css, js) ──────────────────────────
    svr.set_mount_point("/", "./public");

    // ── POST /api/accounts  → create a new account ─────────────────────────
    svr.Post("/api/accounts", [](const httplib::Request& req, httplib::Response& res) {
        std::string name    = extractJsonValue(req.body, "name");
        std::string type    = extractJsonValue(req.body, "type");
        std::string openStr = extractJsonValue(req.body, "opening");

        if (name.empty()) {
            res.set_content(jsonError("Holder name is required."), "application/json");
            return;
        }
        double opening = 0.0;
        try { opening = std::stod(openStr); } catch (...) { opening = 0.0; }

        if (opening < 0) {
            res.set_content(jsonError("Opening deposit cannot be negative."), "application/json");
            return;
        }

        std::string id = generateAccountId();
        Account acc(id, name, type.empty() ? "Savings" : type, opening);

        if (opening > 0) {
            Transaction txn(id + "-TXN-001", "DEPOSIT", opening, opening);
            acc.addTransaction(txn);
            FileHandler::appendTransaction(id, txn);
        }
        FileHandler::saveAccount(acc);

        res.set_content("{\"success\":true,\"account\":" + acc.toJson() + "}",
                         "application/json");
    });

    // ── GET /api/accounts  → list all accounts ──────────────────────────────
    svr.Get("/api/accounts", [](const httplib::Request&, httplib::Response& res) {
        auto ids = FileHandler::listAllAccountIds();
        std::ostringstream oss;
        oss << "{\"success\":true,\"accounts\":[";
        for (size_t i = 0; i < ids.size(); i++) {
            try {
                Account acc = FileHandler::loadAccount(ids[i]);
                oss << acc.toJson();
                if (i != ids.size() - 1) oss << ",";
            } catch (...) {}
        }
        oss << "]}";
        res.set_content(oss.str(), "application/json");
    });

    // ── GET /api/accounts/:id  → account summary ────────────────────────────
    svr.Get(R"(/api/accounts/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (!FileHandler::accountExists(id)) {
            res.status = 404;
            res.set_content(jsonError("Account not found: " + id), "application/json");
            return;
        }
        Account acc = FileHandler::loadAccount(id);
        res.set_content("{\"success\":true,\"account\":" + acc.toJson() + "}",
                         "application/json");
    });

    // ── GET /api/accounts/:id/statement  → full transaction history ─────────
    svr.Get(R"(/api/accounts/([^/]+)/statement)", [](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (!FileHandler::accountExists(id)) {
            res.status = 404;
            res.set_content(jsonError("Account not found: " + id), "application/json");
            return;
        }
        Account acc = FileHandler::loadAccount(id);
        res.set_content(
            "{\"success\":true,\"account\":" + acc.toJson() +
            ",\"transactions\":" + acc.historyToJson() + "}",
            "application/json");
    });

    // ── POST /api/accounts/:id/deposit ──────────────────────────────────────
    svr.Post(R"(/api/accounts/([^/]+)/deposit)", [](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (!FileHandler::accountExists(id)) {
            res.status = 404;
            res.set_content(jsonError("Account not found: " + id), "application/json");
            return;
        }
        std::string amtStr = extractJsonValue(req.body, "amount");
        double amount = 0.0;
        try { amount = std::stod(amtStr); } catch (...) {
            res.set_content(jsonError("Invalid amount."), "application/json");
            return;
        }

        Account acc = FileHandler::loadAccount(id);
        std::string errorMsg;
        if (!acc.deposit(amount, errorMsg)) {
            res.set_content(jsonError(errorMsg), "application/json");
            return;
        }
        FileHandler::saveAccount(acc);
        FileHandler::appendTransaction(id, acc.getHistory().back());

        res.set_content("{\"success\":true,\"account\":" + acc.toJson() + "}",
                         "application/json");
    });

    // ── POST /api/accounts/:id/withdraw ─────────────────────────────────────
    svr.Post(R"(/api/accounts/([^/]+)/withdraw)", [](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (!FileHandler::accountExists(id)) {
            res.status = 404;
            res.set_content(jsonError("Account not found: " + id), "application/json");
            return;
        }
        std::string amtStr = extractJsonValue(req.body, "amount");
        double amount = 0.0;
        try { amount = std::stod(amtStr); } catch (...) {
            res.set_content(jsonError("Invalid amount."), "application/json");
            return;
        }

        Account acc = FileHandler::loadAccount(id);
        std::string errorMsg;
        if (!acc.withdraw(amount, errorMsg)) {
            res.set_content(jsonError(errorMsg), "application/json");
            return;
        }
        FileHandler::saveAccount(acc);
        FileHandler::appendTransaction(id, acc.getHistory().back());

        res.set_content("{\"success\":true,\"account\":" + acc.toJson() + "}",
                         "application/json");
    });

    // ── CORS preflight support (harmless even when served same-origin) ─────
    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS") {
            res.status = 200;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    std::cout << "\n";
    std::cout << "  ============================================\n";
    std::cout << "   Banking System — Web Server\n";
    std::cout << "   Running at: http://localhost:8080\n";
    std::cout << "   Press Ctrl+C to stop\n";
    std::cout << "  ============================================\n\n";

    svr.listen("0.0.0.0", 8080);
    return 0;
}

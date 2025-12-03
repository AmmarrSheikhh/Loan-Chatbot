// bot.cpp - UPDATED for Increment 3 with shared data path
#include "Bot.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>   // for system()
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <string>
#include <regex>
#include <ctime>
#include <unordered_set>

using namespace std;

const std::string SHARED_DATA_PATH = "C:\\Users\\mahru\\source\\repos\\shared_data\\";

ApplicationNode* head = NULL;

// ---------------------- Helper: toUpperCase ----------------------
string toUpperCase(string text) {
    for (size_t i = 0; i < text.size(); ++i)
        if (text[i] >= 'a' && text[i] <= 'z') text[i] -= 32;
    return text;
}

// ---------------------- Helper: strip commas from "1,000,000" ------
static string stripCommas(const string& s) {
    string out;
    out.reserve(s.size());
    for (char c : s) if (c != ',') out.push_back(c);
    return out;
}

// ---------------------- Helper: get filename from path -------------
static string getFileNameOnly(const string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == string::npos) return path;
    return path.substr(pos + 1);
}

// ---------------------- Basic validation helpers -------------------
static bool isDigits(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

static bool isValidCNIC(const string& s) {
    // exactly 13 digits
    return regex_match(s, regex("^[0-9]{13}$"));
}

static bool isValidInteger(const string& s) {
    return regex_match(s, regex("^[0-9]+$"));
}

// ---------------------- Helper: Get loan amount and installments from file -------------------
static pair<double, int> getLoanDetailsFromFile(const string& loanType, const string& selection) {
    string filename;
    if (loanType == "HOME") {
        filename = SHARED_DATA_PATH + "Home.txt";
    }
    else if (loanType == "CAR") {
        filename = SHARED_DATA_PATH + "Car.txt";
    }
    else if (loanType == "SCOOTER") {
        filename = SHARED_DATA_PATH + "Scooter.txt";
    }
    else if (loanType == "PERSONAL") {
        filename = SHARED_DATA_PATH + "Personal.txt";
    }
    else {
        return make_pair(0.0, 0);
    }

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Loan plans file not found: " << filename << "\n";
        return make_pair(0.0, 0);
    }

    string line;
    getline(file, line); // skip header

    // For home loans, selection is area (e.g., "Area 1")
    if (loanType == "HOME") {
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> fields;
            string tmp;
            stringstream ss(line);
            while (getline(ss, tmp, '#')) fields.push_back(tmp);

            if (fields.size() >= 4 && fields[0] == selection) {
                // Price is at index 3, Installments at index 2 in Home.txt
                string priceStr = stripCommas(fields[3]);
                string installmentsStr = fields[2];
                try {
                    double amount = stod(priceStr);
                    int installments = stoi(installmentsStr);
                    return make_pair(amount, installments);
                }
                catch (...) {
                    return make_pair(0.0, 0);
                }
            }
        }
    }
    // For car loans, selection is make/model (simplified)
    else if (loanType == "CAR") {
        int lineCount = 0;
        int selectedLine = stoi(selection) - 1; // User selects by line number

        while (getline(file, line)) {
            if (line.empty()) continue;
            if (lineCount == selectedLine) {
                vector<string> fields;
                string tmp;
                stringstream ss(line);
                while (getline(ss, tmp, '#')) fields.push_back(tmp);

                if (fields.size() >= 8) {
                    // Price is at index 6, Installments at index 5 in Car.txt
                    string priceStr = stripCommas(fields[6]);
                    string installmentsStr = fields[5];
                    try {
                        double amount = stod(priceStr);
                        int installments = stoi(installmentsStr);
                        return make_pair(amount, installments);
                    }
                    catch (...) {
                        return make_pair(0.0, 0);
                    }
                }
            }
            lineCount++;
        }
    }
    // For scooter loans, similar to car loans
    else if (loanType == "SCOOTER") {
        int lineCount = 0;
        int selectedLine = stoi(selection) - 1;

        while (getline(file, line)) {
            if (line.empty()) continue;
            if (lineCount == selectedLine) {
                vector<string> fields;
                string tmp;
                stringstream ss(line);
                while (getline(ss, tmp, '#')) fields.push_back(tmp);

                if (fields.size() >= 8) {
                    // Price is at index 6, Installments at index 5 in Scooter.txt
                    string priceStr = stripCommas(fields[6]);
                    string installmentsStr = fields[5];
                    try {
                        double amount = stod(priceStr);
                        int installments = stoi(installmentsStr);
                        return make_pair(amount, installments);
                    }
                    catch (...) {
                        return make_pair(0.0, 0);
                    }
                }
            }
            lineCount++;
        }
    }
    // For personal loans, selection is line number
    else if (loanType == "PERSONAL") {
        int lineCount = 0;
        int selectedLine = stoi(selection) - 1;

        while (getline(file, line)) {
            if (line.empty()) continue;
            if (lineCount == selectedLine) {
                vector<string> fields;
                string tmp;
                stringstream ss(line);
                while (getline(ss, tmp, '#')) fields.push_back(tmp);

                if (fields.size() >= 2) {
                    // Loan Amount is at index 0, Term at index 1 in Personal.txt
                    string amountStr = stripCommas(fields[0]);
                    string termStr = fields[1];
                    try {
                        double amount = stod(amountStr);
                        int installments = stoi(termStr);
                        return make_pair(amount, installments);
                    }
                    catch (...) {
                        return make_pair(0.0, 0);
                    }
                }
            }
            lineCount++;
        }
    }

    return make_pair(0.0, 0);
}

// date DD-MM-YYYY basic validation (valid calendar date)
static bool isValidDateDDMMYYYY(const string& s) {
    if (!regex_match(s, regex("^\\d{2}-\\d{2}-\\d{4}$"))) return false;
    int d, m, y;
    char dash1, dash2;
    stringstream ss(s);
    ss >> d >> dash1 >> m >> dash2 >> y;
    if (dash1 != '-' || dash2 != '-') return false;
    if (y < 1900 || y > 3000) return false;
    if (m < 1 || m > 12) return false;
    int mdays[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
    bool leap = ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0));
    if (leap) mdays[2] = 29;
    return d >= 1 && d <= mdays[m];
}

static string promptEnum(const string& prompt, const vector<string>& options) {
    while (true) {
        cout << prompt << endl;
        for (size_t i = 0; i < options.size(); ++i) cout << (i + 1) << ". " << options[i] << endl;
        cout << "Choice (enter number): ";
        string choice; getline(cin, choice);
        if (!isDigits(choice)) { cout << "Invalid input. Enter the option number.\n"; continue; }
        int idx = stoi(choice) - 1;
        if (idx < 0 || idx >= (int)options.size()) { cout << "Invalid selection.\n"; continue; }
        return options[idx];
    }
}

// ---------------------- Load triggers/responses -------------------
// unchanged behavior for Utterances.txt equality matching
void loadResponses(string triggers[], string responses[], int& count) {
    string filePath = SHARED_DATA_PATH + "Utterances.txt";
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Warning: Utterances.txt not found at: " << filePath << ". Bot will still run but no custom replies loaded.\n";
        count = 0;
        return;
    }
    string line;
    count = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        int pos = line.find('#');
        if (pos == -1) continue;
        if (count < 100) {
            triggers[count] = toUpperCase(line.substr(0, pos));
            responses[count] = line.substr(pos + 1);
            ++count;
        }
    }
    file.close();
}

// ---------------------- Human chat corpus (IoU) -------------------
static vector<pair<string, string>> loadHumanCorpus(const string& path = "human_chat_corpus.txt") {
    vector<pair<string, string>> pairs;
    string filePath = SHARED_DATA_PATH + path;
    ifstream f(filePath);
    if (!f.is_open()) {
        // no corpus is OK
        return pairs;
    }
    vector<string> lines;
    string line;
    while (getline(f, line)) {
        if (!line.empty())
            lines.push_back(line);
    }
    // lines assumed alternating: Human1, Human2, Human1, Human2...
    for (size_t i = 0; i + 1 < lines.size(); i += 2) {
        pairs.emplace_back(lines[i], lines[i + 1]);
    }
    return pairs;
}

static vector<string> tokenizeLower(const string& s) {
    vector<string> out;
    string token;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (isalnum((unsigned char)c)) token.push_back(tolower(c));
        else {
            if (!token.empty()) { out.push_back(token); token.clear(); }
        }
    }
    if (!token.empty()) out.push_back(token);
    return out;
}

static double iouScore(const string& a, const string& b) {
    auto ta = tokenizeLower(a);
    auto tb = tokenizeLower(b);
    unordered_set<string> sa(ta.begin(), ta.end()), sb(tb.begin(), tb.end());
    if (sa.empty() && sb.empty()) return 0.0;
    int inter = 0;
    for (auto& t : sa) if (sb.count(t)) ++inter;
    int uni = (int)(sa.size() + sb.size() - inter);
    if (uni == 0) return 0.0;
    return (double)inter / (double)uni;
}

static string bestIoUResponse(const string& user, const vector<pair<string, string>>& pairs) {
    double best = 0.0;
    string bestResp = "Sorry, I don't understand.";
    for (auto& p : pairs) {
        double sc = iouScore(user, p.first);
        if (sc > best) { best = sc; bestResp = p.second; }
    }
    return bestResp;
}

// -----------------------------------------------------------
// Monthly Schedule Function - UPDATED to use file amount and installments
// -----------------------------------------------------------
static void printMonthlySchedule(double amount, int months) {
    const string monthsList[12] = {
        "JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE",
        "JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"
    };

    cout << "\nEnter starting month (e.g. January): ";
    string start;
    getline(cin, start);
    string ustart = toUpperCase(start);

    int startIndex = 0;
    for (int i = 0; i < 12; i++) {
        if (ustart == monthsList[i]) {
            startIndex = i;
            break;
        }
    }

    cout << "Enter starting year (e.g. 2025): ";
    string yearStr;
    getline(cin, yearStr);
    int year = 0;
    try {
        year = stoi(yearStr);
    }
    catch (...) {
        year = 2025; // fallback
    }

    cout << fixed << setprecision(2);
    double per = amount / months;

    cout << "\n------------ Monthly Installment Schedule ------------\n";
    cout << "Total Loan Amount: " << amount << " PKR\n";
    cout << "Number of Months: " << months << "\n";
    cout << "Monthly Installment: " << per << " PKR\n";
    cout << "------------------------------------------------------\n";
    cout << left << setw(8) << "No."
        << setw(14) << "Month"
        << setw(8) << "Year"
        << setw(15) << "Installment" << "\n";
    cout << "------------------------------------------------------\n";

    for (int i = 0; i < months; i++) {
        int overallIndex = startIndex + i;
        int m = overallIndex % 12;
        int y = year + (overallIndex / 12);

        cout << left << setw(8) << (i + 1)
            << setw(14) << monthsList[m]
            << setw(8) << y
            << setw(15) << per << "PKR\n";
    }

    cout << "------------------------------------------------------\n";
    cout << "\n(Type MENU anytime to return to main menu)\n";
}

// ---------------------- LOAN PLAN SELECTION -------------------
static pair<string, string> selectLoanPlan() {
    while (true) {
        cout << "\n===== SELECT LOAN TYPE =====\n";
        cout << "H - Home Loan\n";
        cout << "C - Car Loan\n";
        cout << "S - Scooter Loan\n";
        cout << "P - Personal Loan\n";
        cout << "Enter your choice: ";

        string choice;
        getline(cin, choice);
        string upperChoice = toUpperCase(choice);

        if (upperChoice == "H") {
            // Show home loan plans and let user select
            string filePath = SHARED_DATA_PATH + "Home.txt";
            ifstream file(filePath);
            if (!file.is_open()) {
                cout << "Home loan plans file not found.\n";
                continue;
            }

            string line;
            getline(file, line); // skip header
            cout << "\n===== HOME LOAN PLANS =====\n";

            int option = 1;
            vector<string> plans;

            while (getline(file, line)) {
                if (line.empty()) continue;
                plans.push_back(line);

                vector<string> fields;
                string tmp;
                stringstream ss(line);
                while (getline(ss, tmp, '#')) fields.push_back(tmp);

                if (fields.size() >= 5) {
                    cout << option << ". " << fields[0] << " - " << fields[1] << " - " << fields[3] << " PKR - " << fields[2] << " months\n";
                }
                option++;
            }
            file.close();

            if (plans.empty()) {
                cout << "No home loan plans available.\n";
                continue;
            }

            cout << "Select plan (enter number): ";
            string planChoice;
            getline(cin, planChoice);

            if (!isDigits(planChoice) || stoi(planChoice) < 1 || stoi(planChoice) > plans.size()) {
                cout << "Invalid selection.\n";
                continue;
            }

            // Get the selected plan details
            string selectedLine = plans[stoi(planChoice) - 1];
            vector<string> fields;
            string tmp;
            stringstream ss(selectedLine);
            while (getline(ss, tmp, '#')) fields.push_back(tmp);

            string planDetails = fields[0] + " - " + fields[1] + " - " + fields[3] + " PKR";
            return make_pair("HOME", planDetails);
        }
        else if (upperChoice == "C") {
            // Show car loan plans and let user select
            string filePath = SHARED_DATA_PATH + "Car.txt";
            ifstream file(filePath);
            if (!file.is_open()) {
                cout << "Car loan plans file not found.\n";
                continue;
            }

            string line;
            getline(file, line); // skip header
            cout << "\n===== CAR LOAN PLANS =====\n";

            int option = 1;
            vector<string> plans;

            while (getline(file, line)) {
                if (line.empty()) continue;
                plans.push_back(line);

                vector<string> fields;
                string tmp;
                stringstream ss(line);
                while (getline(ss, tmp, '#')) fields.push_back(tmp);

                if (fields.size() >= 8) {
                    string used = (fields[3] == "No") ? "New" : "Used";
                    cout << option << ". " << fields[0] << " " << fields[1] << " - " << fields[2] << "cc - " << used << " - " << fields[6] << " PKR\n";
                }
                option++;
            }
            file.close();

            if (plans.empty()) {
                cout << "No car loan plans available.\n";
                continue;
            }

            cout << "Select plan (enter number): ";
            string planChoice;
            getline(cin, planChoice);

            if (!isDigits(planChoice) || stoi(planChoice) < 1 || stoi(planChoice) > plans.size()) {
                cout << "Invalid selection.\n";
                continue;
            }

            // Get the selected plan details
            string selectedLine = plans[stoi(planChoice) - 1];
            vector<string> fields;
            string tmp;
            stringstream ss(selectedLine);
            while (getline(ss, tmp, '#')) fields.push_back(tmp);

            string used = (fields[3] == "No") ? "New" : "Used";
            string planDetails = fields[0] + " " + fields[1] + " - " + fields[2] + "cc - " + used + " - " + fields[6] + " PKR";
            return make_pair("CAR", planDetails);
        }
        else if (upperChoice == "S") {
            // Show scooter loan plans and let user select
            string filePath = SHARED_DATA_PATH + "Scooter.txt";
            ifstream file(filePath);
            if (!file.is_open()) {
                cout << "Scooter loan plans file not found.\n";
                continue;
            }

            string line;
            getline(file, line); // skip header
            cout << "\n===== SCOOTER LOAN PLANS =====\n";

            int option = 1;
            vector<string> plans;

            while (getline(file, line)) {
                if (line.empty()) continue;
                plans.push_back(line);

                vector<string> fields;
                string tmp;
                stringstream ss(line);
                while (getline(ss, tmp, '#')) fields.push_back(tmp);

                if (fields.size() >= 8) {
                    cout << option << ". " << fields[0] << " " << fields[1] << " - " << fields[2] << "km range - " << fields[6] << " PKR\n";
                }
                option++;
            }
            file.close();

            if (plans.empty()) {
                cout << "No scooter loan plans available.\n";
                continue;
            }

            cout << "Select plan (enter number): ";
            string planChoice;
            getline(cin, planChoice);

            if (!isDigits(planChoice) || stoi(planChoice) < 1 || stoi(planChoice) > plans.size()) {
                cout << "Invalid selection.\n";
                continue;
            }

            // Get the selected plan details
            string selectedLine = plans[stoi(planChoice) - 1];
            vector<string> fields;
            string tmp;
            stringstream ss(selectedLine);
            while (getline(ss, tmp, '#')) fields.push_back(tmp);

            string planDetails = fields[0] + " " + fields[1] + " - " + fields[2] + "km range - " + fields[6] + " PKR";
            return make_pair("SCOOTER", planDetails);
        }
        else if (upperChoice == "P") {
            // Show personal loan plans and let user select
            string filePath = SHARED_DATA_PATH + "Personal.txt";
            ifstream file(filePath);
            if (!file.is_open()) {
                cout << "Personal loan plans file not found.\n";
                continue;
            }

            string line;
            getline(file, line); // skip header
            cout << "\n===== PERSONAL LOAN PLANS =====\n";

            int option = 1;
            vector<string> plans;

            while (getline(file, line)) {
                if (line.empty()) continue;
                plans.push_back(line);

                vector<string> fields;
                string tmp;
                stringstream ss(line);
                while (getline(ss, tmp, '#')) fields.push_back(tmp);

                if (fields.size() >= 5) {
                    cout << option << ". " << fields[0] << " - " << fields[1] << " months - " << fields[2] << " interest\n";
                }
                option++;
            }
            file.close();

            if (plans.empty()) {
                cout << "No personal loan plans available.\n";
                continue;
            }

            cout << "Select plan (enter number): ";
            string planChoice;
            getline(cin, planChoice);

            if (!isDigits(planChoice) || stoi(planChoice) < 1 || stoi(planChoice) > plans.size()) {
                cout << "Invalid selection.\n";
                continue;
            }

            // Get the selected plan details
            string selectedLine = plans[stoi(planChoice) - 1];
            vector<string> fields;
            string tmp;
            stringstream ss(selectedLine);
            while (getline(ss, tmp, '#')) fields.push_back(tmp);

            string planDetails = fields[0] + " - " + fields[1] + " months - " + fields[2] + " interest";
            return make_pair("PERSONAL", planDetails);
        }
        else {
            cout << "Invalid choice. Please enter H, C, S, or P.\n";
        }
    }
}

// ---------------------- LOAN PLAN FUNCTIONS - UPDATED with proper column alignment -------------------
void showHomeLoanPlans() {
    string filePath = SHARED_DATA_PATH + "Home.txt";
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Home loan plans file not found at: " << filePath << "\n";
        return;
    }

    string line;
    getline(file, line); // skip header
    cout << "\n===== HOME LOAN PLANS =====\n";
    cout << left << setw(8) << "Option"
        << setw(12) << "Area"
        << setw(15) << "Size"
        << setw(15) << "Installments"
        << setw(18) << "Price (PKR)"
        << setw(18) << "Down Payment (PKR)" << endl;
    cout << string(80, '-') << endl;

    int option = 1;
    vector<string> options;

    while (getline(file, line)) {
        if (line.empty()) continue;
        options.push_back(line);

        vector<string> fields;
        string tmp;
        stringstream ss(line);
        while (getline(ss, tmp, '#')) fields.push_back(tmp);

        if (fields.size() >= 5) {
            cout << left << setw(8) << (to_string(option) + ".")
                << setw(12) << (fields[0].length() > 10 ? fields[0].substr(0, 10) : fields[0])
                << setw(15) << (fields[1].length() > 12 ? fields[1].substr(0, 12) : fields[1])
                << setw(15) << fields[2]
                << setw(18) << fields[3]
                << setw(18) << fields[4] << endl;
        }
        option++;
    }
    file.close();

    // Ask if user wants to generate payment schedule
    cout << "\nDo you want to generate a payment schedule? (YES/NO): ";
    string choice;
    getline(cin, choice);

    if (toUpperCase(choice) == "YES") {
        cout << "Select plan (enter number): ";
        string planChoice;
        getline(cin, planChoice);

        if (!isDigits(planChoice) || stoi(planChoice) < 1 || stoi(planChoice) >= option) {
            cout << "Invalid selection.\n";
            return;
        }

        // Get the selected area from the chosen plan
        string selectedLine = options[stoi(planChoice) - 1];
        vector<string> fields;
        string tmp;
        stringstream ss(selectedLine);
        while (getline(ss, tmp, '#')) fields.push_back(tmp);

        if (fields.size() >= 1) {
            string area = fields[0];
            auto loanDetails = getLoanDetailsFromFile("HOME", area);
            double amount = loanDetails.first;
            int installments = loanDetails.second;

            if (amount > 0 && installments > 0) {
                cout << "\nGenerating payment schedule for " << installments << " months...\n";
                printMonthlySchedule(amount, installments);
            }
            else {
                cout << "Could not determine loan details.\n";
            }
        }
    }
}

void showCarLoanPlans() {
    string filePath = SHARED_DATA_PATH + "Car.txt";
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Car loan plans file not found at: " << filePath << "\n";
        return;
    }

    string line;
    getline(file, line); // skip header
    cout << "\n===== CAR LOAN PLANS =====\n";
    cout << left << setw(8) << "Option"
        << setw(12) << "Make"
        << setw(15) << "Model"
        << setw(12) << "Engine"
        << setw(8) << "Used"
        << setw(12) << "Year"
        << setw(15) << "Installments"
        << setw(18) << "Price (PKR)"
        << setw(18) << "Down Payment (PKR)" << endl;
    cout << string(120, '-') << endl;

    int option = 1;
    vector<string> options;

    while (getline(file, line)) {
        if (line.empty()) continue;
        options.push_back(line);

        vector<string> fields;
        string tmp;
        stringstream ss(line);
        while (getline(ss, tmp, '#')) fields.push_back(tmp);

        // Fixed: Check for 8 fields (not 9)
        if (fields.size() >= 8) {
            cout << left << setw(8) << (to_string(option) + ".")
                << setw(12) << (fields[0].length() > 10 ? fields[0].substr(0, 10) : fields[0])
                << setw(15) << (fields[1].length() > 12 ? fields[1].substr(0, 12) : fields[1])
                << setw(12) << fields[2]
                << setw(8) << fields[3]
                << setw(12) << fields[4]
                << setw(15) << fields[5]
                << setw(18) << fields[6]
                << setw(18) << fields[7] << endl;
        }
        option++;
    }
    file.close();

    // Ask if user wants to generate payment schedule
    cout << "\nDo you want to generate a payment schedule? (YES/NO): ";
    string choice;
    getline(cin, choice);

    if (toUpperCase(choice) == "YES") {
        cout << "Select plan (enter number): ";
        string planChoice;
        getline(cin, planChoice);

        if (!isDigits(planChoice) || stoi(planChoice) < 1 || stoi(planChoice) >= option) {
            cout << "Invalid selection.\n";
            return;
        }

        auto loanDetails = getLoanDetailsFromFile("CAR", planChoice);
        double amount = loanDetails.first;
        int installments = loanDetails.second;

        if (amount > 0 && installments > 0) {
            cout << "\nGenerating payment schedule for " << installments << " months...\n";
            printMonthlySchedule(amount, installments);
        }
        else {
            cout << "Could not determine loan details.\n";
        }
    }
}

void showScooterLoanPlans() {
    string filePath = SHARED_DATA_PATH + "Scooter.txt";
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Scooter loan plans file not found at: " << filePath << "\n";
        return;
    }

    string line;
    getline(file, line); // skip header
    cout << "\n===== SCOOTER LOAN PLANS =====\n";
    cout << left << setw(8) << "Option"
        << setw(12) << "Make"
        << setw(15) << "Model"
        << setw(12) << "Range(KM)"
        << setw(15) << "Charge Time(Hrs)"
        << setw(12) << "Max Speed"
        << setw(15) << "Installments"
        << setw(18) << "Price (PKR)"
        << setw(18) << "Down Payment (PKR)" << endl;
    cout << string(125, '-') << endl;

    int option = 1;
    vector<string> options;

    while (getline(file, line)) {
        if (line.empty()) continue;
        options.push_back(line);

        vector<string> fields;
        string tmp;
        stringstream ss(line);
        while (getline(ss, tmp, '#')) fields.push_back(tmp);

        // Fixed: Check for 8 fields (not 9)
        if (fields.size() >= 8) {
            cout << left << setw(8) << (to_string(option) + ".")
                << setw(12) << (fields[0].length() > 10 ? fields[0].substr(0, 10) : fields[0])
                << setw(15) << (fields[1].length() > 12 ? fields[1].substr(0, 12) : fields[1])
                << setw(12) << fields[2]
                << setw(15) << fields[3]
                << setw(12) << fields[4]
                << setw(15) << fields[5]
                << setw(18) << fields[6]
                << setw(18) << fields[7] << endl;
        }
        option++;
    }
    file.close();

    // Ask if user wants to generate payment schedule
    cout << "\nDo you want to generate a payment schedule? (YES/NO): ";
    string choice;
    getline(cin, choice);

    if (toUpperCase(choice) == "YES") {
        cout << "Select plan (enter number): ";
        string planChoice;
        getline(cin, planChoice);

        if (!isDigits(planChoice) || stoi(planChoice) < 1 || stoi(planChoice) >= option) {
            cout << "Invalid selection.\n";
            return;
        }

        auto loanDetails = getLoanDetailsFromFile("SCOOTER", planChoice);
        double amount = loanDetails.first;
        int installments = loanDetails.second;

        if (amount > 0 && installments > 0) {
            cout << "\nGenerating payment schedule for " << installments << " months...\n";
            printMonthlySchedule(amount, installments);
        }
        else {
            cout << "Could not determine loan details.\n";
        }
    }
}

void showPersonalLoanPlans() {
    string filePath = SHARED_DATA_PATH + "Personal.txt";
    ifstream file(filePath);

    cout << "\n===== PERSONAL LOAN PLANS =====\n";

    if (file.is_open()) {
        string line;
        getline(file, line); // skip header
        cout << left << setw(8) << "Option"
            << setw(20) << "Loan Amount (PKR)"
            << setw(15) << "Term (months)"
            << setw(18) << "Interest Rate"
            << setw(20) << "Monthly Payment (PKR)"
            << setw(18) << "Total Payment (PKR)" << endl;
        cout << string(85, '-') << endl;

        int option = 1;
        vector<string> options;

        while (getline(file, line)) {
            if (line.empty()) continue;
            options.push_back(line);

            vector<string> fields;
            string tmp;
            stringstream ss(line);
            while (getline(ss, tmp, '#')) fields.push_back(tmp);

            if (fields.size() >= 5) {
                cout << left << setw(8) << (to_string(option) + ".")
                    << setw(20) << fields[0]
                    << setw(15) << fields[1]
                    << setw(18) << fields[2]
                    << setw(20) << fields[3]
                    << setw(18) << fields[4] << endl;
            }
            option++;
        }
        file.close();

        // Ask if user wants to generate payment schedule
        cout << "\nDo you want to generate a payment schedule? (YES/NO): ";
        string choice;
        getline(cin, choice);

        if (toUpperCase(choice) == "YES") {
            cout << "Select plan (enter number): ";
            string planChoice;
            getline(cin, planChoice);

            if (!isDigits(planChoice) || stoi(planChoice) < 1 || stoi(planChoice) >= option) {
                cout << "Invalid selection.\n";
                return;
            }

            auto loanDetails = getLoanDetailsFromFile("PERSONAL", planChoice);
            double amount = loanDetails.first;
            int installments = loanDetails.second;

            if (amount > 0 && installments > 0) {
                cout << "\nGenerating payment schedule for " << installments << " months...\n";
                printMonthlySchedule(amount, installments);
            }
            else {
                cout << "Could not determine loan details.\n";
            }
        }
    }
    else {
        cout << "Personal loans available from 100,000 PKR to 5,000,000 PKR\n";
        cout << "Terms: 12 to 60 months\n";
        cout << "Interest rates: 12% to 18% based on credit assessment\n";
        cout << "Please contact our representatives for personalized quotes.\n";
    }
}

// ---------------------- IMAGE COPY — store under data/<AppID>/<originalFilename>
void copyImageToDataFolder(const string& srcPath, const string& appID) {
    if (srcPath.empty()) return;

    // Ensure base data folder exists in shared location
    string dataFolder = SHARED_DATA_PATH + "data";
    string mkdirCmd = "mkdir \"" + dataFolder + "\" >nul 2>nul";
    system(mkdirCmd.c_str());

    // Create per-application folder: data\<AppID>
    string appFolder = dataFolder + "\\" + appID;
    string mkdirCmd2 = "mkdir \"" + appFolder + "\" >nul 2>nul";
    system(mkdirCmd2.c_str());

    string fileName = getFileNameOnly(srcPath);  // original filename
    string destPath = appFolder + "\\" + fileName;

    ifstream src(srcPath, ios::binary);
    ofstream dest(destPath, ios::binary);
    if (!src.is_open() || !dest.is_open()) {
        cout << "Warning: Could not copy image: " << srcPath << " -> " << destPath << "\n";
        return;
    }
    dest << src.rdbuf();
    src.close();
    dest.close();
}

// ---------------------- APPLICATION STORAGE HELPERS --------------
static vector<vector<string>> loadAllRawApplications() {
    vector<vector<string>> out;
    string filePath = SHARED_DATA_PATH + "applications.txt";
    ifstream f(filePath);
    if (!f.is_open()) return out;
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        vector<string> fields; string tmp; stringstream ss(line);
        while (getline(ss, tmp, '#')) fields.push_back(tmp);
        out.push_back(fields);
    }
    f.close();
    return out;
}

static void writeAllRawApplications(const vector<vector<string>>& apps) {
    string filePath = SHARED_DATA_PATH + "applications.txt";
    ofstream out(filePath, ios::trunc);
    for (auto& fields : apps) {
        for (size_t i = 0; i < fields.size(); ++i) {
            out << fields[i];
            if (i + 1 < fields.size()) out << "#";
        }
        out << "\n";
    }
    out.close();
}

// helper: find index by appID (appID stored at second-last field as before)
// helper: find index by appID (appID stored at field 35)
static int findRawAppIndexByID(const vector<vector<string>>& apps, const string& appID) {
    for (size_t i = 0; i < apps.size(); ++i) {
        auto& f = apps[i];
        if (f.size() > 35 && f[35] == appID) return (int)i;
    }
    return -1;
}

// helper: find raw app index by appID + CNIC (CNIC at index 5, AppID at index 35)
static int findRawAppIndexByIDAndCNIC(const vector<vector<string>>& apps, const string& appID, const string& cnic) {
    for (size_t i = 0; i < apps.size(); ++i) {
        auto& f = apps[i];
        // Check if we have enough fields and if CNIC (index 5) and AppID (index 35) match
        if (f.size() > 35 && f[5] == cnic && f[35] == appID) {
            return (int)i;
        }
    }
    return -1;
}

// Generate human-friendly new application ID (4-digit)
string generateAppID() {
    auto apps = loadAllRawApplications();
    int count = (int)apps.size() + 1;
    string id = to_string(count);
    while (id.length() < 4) id = "0" + id;
    return id;
}

// construct raw fields vector from ApplicationNode (same order as saveApplicationToFile earlier)
static vector<string> fieldsFromNode(const ApplicationNode& n) {
    vector<string> f;
    f.push_back(n.fullName);
    f.push_back(n.fatherName);
    f.push_back(n.postalAddress);
    f.push_back(n.contactNumber);
    f.push_back(n.email);
    f.push_back(n.cnic);
    f.push_back(n.cnicExpiry);
    f.push_back(n.employmentStatus);
    f.push_back(n.maritalStatus);
    f.push_back(n.gender);
    f.push_back(n.dependents);
    f.push_back(n.annualIncome);
    f.push_back(n.avgElectricityBill);
    f.push_back(n.currentElectricBill);
    f.push_back(n.hasPrevLoan);
    f.push_back(n.prevLoanActive);
    f.push_back(n.prevLoanTotal);
    f.push_back(n.prevLoanReturned);
    f.push_back(n.prevLoanDue);
    f.push_back(n.prevLoanBank);
    f.push_back(n.prevLoanCategory);
    f.push_back(n.ref1Name);
    f.push_back(n.ref1CNIC);
    f.push_back(n.ref1Issue);
    f.push_back(n.ref1Phone);
    f.push_back(n.ref1Email);
    f.push_back(n.ref2Name);
    f.push_back(n.ref2CNIC);
    f.push_back(n.ref2Issue);
    f.push_back(n.ref2Phone);
    f.push_back(n.ref2Email);
    f.push_back(n.cnicFrontImg);
    f.push_back(n.cnicBackImg);
    f.push_back(n.billImg);
    f.push_back(n.salaryImg);
    f.push_back(n.applicationID);
    f.push_back(n.status);
    f.push_back(n.loanType);
    f.push_back(n.loanPlanDetails);  // ADD THIS LINE
    return f;
}

// ---------------------- MISSING FUNCTION IMPLEMENTATIONS -------------------

ApplicationNode* createApplicationNode() {
    ApplicationNode* newNode = new ApplicationNode;
    // Initialize all fields to empty
    newNode->fullName = "";
    newNode->fatherName = "";
    newNode->postalAddress = "";
    newNode->contactNumber = "";
    newNode->email = "";
    newNode->cnic = "";
    newNode->cnicExpiry = "";
    newNode->employmentStatus = "";
    newNode->maritalStatus = "";
    newNode->gender = "";
    newNode->dependents = "";
    newNode->annualIncome = "";
    newNode->avgElectricityBill = "";
    newNode->currentElectricBill = "";
    newNode->hasPrevLoan = "";
    newNode->prevLoanActive = "";
    newNode->prevLoanTotal = "";
    newNode->prevLoanReturned = "";
    newNode->prevLoanDue = "";
    newNode->prevLoanBank = "";
    newNode->prevLoanCategory = "";
    newNode->ref1Name = "";
    newNode->ref1CNIC = "";
    newNode->ref1Issue = "";
    newNode->ref1Phone = "";
    newNode->ref1Email = "";
    newNode->ref2Name = "";
    newNode->ref2CNIC = "";
    newNode->ref2Issue = "";
    newNode->ref2Phone = "";
    newNode->ref2Email = "";
    newNode->cnicFrontImg = "";
    newNode->cnicBackImg = "";
    newNode->billImg = "";
    newNode->salaryImg = "";
    newNode->applicationID = "";
    newNode->status = "";
    newNode->loanType = "";
    newNode->loanPlanDetails = "";  // Initialize loan plan details
    newNode->next = nullptr;
    return newNode;
}

void insertNode(ApplicationNode* node) {
    if (head == NULL) {
        head = node;
    }
    else {
        ApplicationNode* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node;
    }
}

void processFullApplication() {
    cout << "\nStarting full application process...\n";
    processMultiSessionApplication();
}

// ---------------------- SECTION COLLECTION ------------------------
static void collectPersonalSection(ApplicationNode& n) {
    cout << "\n--- Personal Information ---\n";
    cout << "Full Name: "; getline(cin, n.fullName);
    cout << "Father's Name: "; getline(cin, n.fatherName);
    cout << "Postal Address: "; getline(cin, n.postalAddress);

    while (true) {
        cout << "Contact Number (digits only): ";
        getline(cin, n.contactNumber);
        if (isDigits(n.contactNumber) && n.contactNumber.size() >= 7) break;
        cout << "Invalid contact number.\n";
    }

    cout << "Email: "; getline(cin, n.email);

    while (true) {
        cout << "CNIC (13 digits, no dashes): ";
        getline(cin, n.cnic);
        if (isValidCNIC(n.cnic)) break;
        cout << "Invalid CNIC. Must be 13 digits.\n";
    }

    while (true) {
        cout << "CNIC Expiry (DD-MM-YYYY): ";
        getline(cin, n.cnicExpiry);
        if (isValidDateDDMMYYYY(n.cnicExpiry)) break;
        cout << "Invalid date format or invalid date.\n";
    }

    n.employmentStatus = promptEnum("Employment status:", { "Self-employed","Salaried","Retired","Unemployed" });
    n.maritalStatus = promptEnum("Marital status:", { "Single","Married","Divorced","Widowed" });
    n.gender = promptEnum("Gender:", { "Male","Female","Other" });

    while (true) {
        cout << "Number of dependents (0 if none): ";
        getline(cin, n.dependents);
        if (isValidInteger(n.dependents)) break;
        cout << "Invalid number.\n";
    }
}

static void collectFinancialSection(ApplicationNode& n) {
    cout << "\n--- Financial Information ---\n";

    while (true) {
        cout << "Annual income in PKR (numeric, no commas): ";
        getline(cin, n.annualIncome);
        if (isValidInteger(n.annualIncome)) break;
        cout << "Invalid amount.\n";
    }
    while (true) {
        cout << "Monthly avg electricity bill (last 12 months) PKR (numeric): ";
        getline(cin, n.avgElectricityBill);
        if (isValidInteger(n.avgElectricityBill)) break;
        cout << "Invalid amount.\n";
    }
    while (true) {
        cout << "Current electricity bill PKR (numeric): ";
        getline(cin, n.currentElectricBill);
        if (isValidInteger(n.currentElectricBill)) break;
        cout << "Invalid amount.\n";
    }

    cout << "Previous Loan? (YES/NO): ";
    getline(cin, n.hasPrevLoan);
    if (toUpperCase(n.hasPrevLoan) == "YES") {
        cout << "Active? (YES/NO): "; getline(cin, n.prevLoanActive);
        while (true) {
            cout << "Total Loan Amount PKR: "; getline(cin, n.prevLoanTotal);
            if (isValidInteger(n.prevLoanTotal)) break;
            cout << "Invalid amount.\n";
        }
        while (true) {
            cout << "Amount returned PKR: "; getline(cin, n.prevLoanReturned);
            if (isValidInteger(n.prevLoanReturned)) break;
            cout << "Invalid amount.\n";
        }
        while (true) {
            cout << "Amount still due PKR: "; getline(cin, n.prevLoanDue);
            if (isValidInteger(n.prevLoanDue)) break;
            cout << "Invalid amount.\n";
        }
        cout << "Bank: "; getline(cin, n.prevLoanBank);
        n.prevLoanCategory = promptEnum("Loan category:", { "Car","Home","Bike" });
    }
    else {
        n.prevLoanActive = "NO";
        n.prevLoanTotal = "0";
        n.prevLoanReturned = "0";
        n.prevLoanDue = "0";
        n.prevLoanBank = "";
        n.prevLoanCategory = "";
    }
}

static void collectReferencesSection(ApplicationNode& n) {
    cout << "\n--- References ---\n";
    cout << "--- Reference 1 ---\n";
    cout << "Name: "; getline(cin, n.ref1Name);
    while (true) {
        cout << "CNIC (13 digits): "; getline(cin, n.ref1CNIC);
        if (isValidCNIC(n.ref1CNIC)) break;
        cout << "Invalid CNIC.\n";
    }
    while (true) {
        cout << "Issue Date (DD-MM-YYYY): "; getline(cin, n.ref1Issue);
        if (isValidDateDDMMYYYY(n.ref1Issue)) break;
        cout << "Invalid date.\n";
    }
    cout << "Phone: "; getline(cin, n.ref1Phone);
    cout << "Email: "; getline(cin, n.ref1Email);

    cout << "\n--- Reference 2 ---\n";
    cout << "Name: "; getline(cin, n.ref2Name);
    while (true) {
        cout << "CNIC (13 digits): "; getline(cin, n.ref2CNIC);
        if (isValidCNIC(n.ref2CNIC)) break;
        cout << "Invalid CNIC.\n";
    }
    while (true) {
        cout << "Issue Date (DD-MM-YYYY): "; getline(cin, n.ref2Issue);
        if (isValidDateDDMMYYYY(n.ref2Issue)) break;
        cout << "Invalid date.\n";
    }
    cout << "Phone: "; getline(cin, n.ref2Phone);
    cout << "Email: "; getline(cin, n.ref2Email);
}

static void collectDocumentsSection(ApplicationNode& n) {
    cout << "\n--- Documents (supply file paths) ---\n";
    cout << "Image Path (CNIC Front): "; getline(cin, n.cnicFrontImg);
    cout << "Image Path (CNIC Back): "; getline(cin, n.cnicBackImg);
    cout << "Image Path (Electricity Bill): "; getline(cin, n.billImg);
    cout << "Image Path (Salary Slip): "; getline(cin, n.salaryImg);
}

// ---------------------- Save / Upsert Application -----------------
static void upsertApplicationToFile(ApplicationNode& n) {
    auto apps = loadAllRawApplications();
    // find existing by applicationID (if set)
    int idx = -1;
    if (!n.applicationID.empty()) idx = findRawAppIndexByID(apps, n.applicationID);

    if (idx == -1) {
        // new record: set appID and push back
        n.applicationID = generateAppID();
        // status must be either C1/C2/C3/Submitted
        if (n.status.empty()) n.status = "C1";
        auto f = fieldsFromNode(n);
        apps.push_back(f);
    }
    else {
        // update existing entry
        apps[idx] = fieldsFromNode(n);
    }
    writeAllRawApplications(apps);

    // copy images if provided
    if (!n.cnicFrontImg.empty()) copyImageToDataFolder(n.cnicFrontImg, n.applicationID);
    if (!n.cnicBackImg.empty()) copyImageToDataFolder(n.cnicBackImg, n.applicationID);
    if (!n.billImg.empty()) copyImageToDataFolder(n.billImg, n.applicationID);
    if (!n.salaryImg.empty()) copyImageToDataFolder(n.salaryImg, n.applicationID);
}

// load an application into an ApplicationNode by ID
static bool loadApplicationNodeByID(ApplicationNode& n, const string& appID) {
    auto apps = loadAllRawApplications();
    int idx = findRawAppIndexByID(apps, appID);
    if (idx == -1) return false;
    auto& f = apps[idx];
    // Defensive: copy fields into node by index if present
    auto getField = [&](int i)->string { return (i < (int)f.size() ? f[i] : string()); };
    n.fullName = getField(0);
    n.fatherName = getField(1);
    n.postalAddress = getField(2);
    n.contactNumber = getField(3);
    n.email = getField(4);
    n.cnic = getField(5);
    n.cnicExpiry = getField(6);
    n.employmentStatus = getField(7);
    n.maritalStatus = getField(8);
    n.gender = getField(9);
    n.dependents = getField(10);
    n.annualIncome = getField(11);
    n.avgElectricityBill = getField(12);
    n.currentElectricBill = getField(13);
    n.hasPrevLoan = getField(14);
    n.prevLoanActive = getField(15);
    n.prevLoanTotal = getField(16);
    n.prevLoanReturned = getField(17);
    n.prevLoanDue = getField(18);
    n.prevLoanBank = getField(19);
    n.prevLoanCategory = getField(20);
    n.ref1Name = getField(21);
    n.ref1CNIC = getField(22);
    n.ref1Issue = getField(23);
    n.ref1Phone = getField(24);
    n.ref1Email = getField(25);
    n.ref2Name = getField(26);
    n.ref2CNIC = getField(27);
    n.ref2Issue = getField(28);
    n.ref2Phone = getField(29);
    n.ref2Email = getField(30);
    n.cnicFrontImg = getField(31);
    n.cnicBackImg = getField(32);
    n.billImg = getField(33);
    n.salaryImg = getField(34);
    n.applicationID = getField(35);
    n.status = getField(36);
    n.loanType = getField(37);
    n.loanPlanDetails = getField(38);  // ADD THIS LINE - load loan plan details
    return true;
}

// ---------------------- PROCESS MULTI-SESSION APPLICATION -------
void processMultiSessionApplication() {
    cout << "\nStart new application or resume existing?\n1. New\n2. Resume\nChoice: ";
    string choice; getline(cin, choice);
    if (choice == "2") {
        cout << "Enter application ID: "; string aid; getline(cin, aid);
        cout << "Enter CNIC (no dashes): "; string cnic; getline(cin, cnic);
        if (!isValidCNIC(cnic)) { cout << "Invalid CNIC format.\n"; return; }
        // verify match
        ApplicationNode node;
        if (!loadApplicationNodeByID(node, aid)) { cout << "Application ID not found.\n"; return; }
        if (node.cnic != cnic) { cout << "CNIC does not match application owner.\n"; return; }
        if (toUpperCase(node.status) == "SUBMITTED") { cout << "Application already submitted; cannot modify.\n"; return; }

        cout << "\nResuming Application ID: " << aid << " | Status: " << node.status << "\n";
        cout << "Loan Plan: " << node.loanPlanDetails << "\n";

        // decide where to resume based on status
        if (node.status == "C1") {
            cout << "You have completed Personal section. Next: Financial.\n";
            collectFinancialSection(node);
            node.status = "C2"; upsertApplicationToFile(node);
            cout << "Saved Financial section (C2).\n";
            cout << "Do you want to continue to References? (YES/NO): ";
            string yn; getline(cin, yn);
            if (toUpperCase(yn) == "YES") { collectReferencesSection(node); node.status = "C3"; upsertApplicationToFile(node); cout << "Saved References (C3)\n"; }
        }
        else if (node.status == "C2") {
            cout << "You have completed Financial section. Next: References.\n";
            collectReferencesSection(node); node.status = "C3"; upsertApplicationToFile(node); cout << "Saved References (C3)\n";
        }
        else if (node.status == "C3") {
            cout << "You have completed References. Next: Documents.\n";
        }
        else {
            // unknown intermediate state: allow user to fill missing sections manually
        }

        // Prompt for Documents if not yet provided
        if (node.status != "Submitted") {
            cout << "Do you want to add/complete Documents now? (YES/NO): ";
            string yn; getline(cin, yn);
            if (toUpperCase(yn) == "YES") {
                collectDocumentsSection(node);
                node.status = "Submitted";
                upsertApplicationToFile(node);
                cout << "All sections complete. Application Submitted.\n";
            }
            else {
                cout << "You can resume later. Current status: " << node.status << "\n";
            }
        }
    }
    else {
        // New application flow with checkpoints after each section
        ApplicationNode node;

        // ADDED: Loan type and plan selection at start
        auto loanSelection = selectLoanPlan();
        node.loanType = loanSelection.first;
        node.loanPlanDetails = loanSelection.second;

        cout << "\nYou are applying for: " << node.loanPlanDetails << "\n";
        node.status = "C1"; // when personal done, set C1
        collectPersonalSection(node);
        // save checkpoint after personal
        upsertApplicationToFile(node);
        cout << "Personal section saved (C1). Your application ID: " << node.applicationID << "\n";
        cout << "Continue to Financial? (YES/NO): ";
        string yn; getline(cin, yn);
        if (toUpperCase(yn) == "YES") {
            collectFinancialSection(node);
            node.status = "C2";
            upsertApplicationToFile(node);
            cout << "Financial section saved (C2).\n";
            cout << "Continue to References? (YES/NO): ";
            getline(cin, yn);
            if (toUpperCase(yn) == "YES") {
                collectReferencesSection(node);
                node.status = "C3";
                upsertApplicationToFile(node);
                cout << "References saved (C3).\n";
                cout << "Continue to Documents? (YES/NO): ";
                getline(cin, yn);
                if (toUpperCase(yn) == "YES") {
                    collectDocumentsSection(node);
                    node.status = "Submitted";
                    upsertApplicationToFile(node);
                    cout << "Documents saved and Application Submitted.\n";
                }
                else {
                    cout << "You can resume later. Current status: " << node.status << "\n";
                }
            }
            else {
                cout << "You can resume later. Current status: " << node.status << "\n";
            }
        }
        else {
            cout << "You can resume later. Current status: " << node.status << "\n";
        }
    }
}

// ---------------------- check application status -----------------
void checkApplicationStatus(const string& appID) {
    auto apps = loadAllRawApplications();
    int idx = findRawAppIndexByID(apps, appID);
    if (idx == -1) { cout << "Application ID not found.\n"; return; }
    auto& f = apps[idx];
    string name = (f.size() >= 1 ? f[0] : "<no name>");
    string status = (f.size() >= 1 ? f.back() : string());
    string loanType = "Unknown";
    string loanPlan = "No plan selected";
    if (f.size() > 37) {
        loanType = f[37];
    }
    if (f.size() > 38) {
        loanPlan = f[38];
    }
    cout << "\nApplication ID: " << appID
        << "\nName: " << name
        << "\nLoan Type: " << loanType
        << "\nLoan Plan: " << loanPlan
        << "\nStatus: " << status << "\n";
}

// ---------------------- CNIC-based counts -------------------
void countApplicationsByCNIC(const string& cnic) {
    auto apps = loadAllRawApplications();
    int submitted = 0, approved = 0, rejected = 0;
    for (auto& f : apps) {
        if (f.size() < 7) continue;
        string fileCNIC = f[5];
        string status = f.back();
        if (fileCNIC == cnic) {
            string s = toUpperCase(status);
            if (s == "SUBMITTED") submitted++;
            else if (s == "APPROVED") approved++;
            else if (s == "REJECTED") rejected++;
        }
    }
    cout << "\nCounts for CNIC " << cnic << ":\n";
    cout << "Submitted: " << submitted << "\n";
    cout << "Approved : " << approved << "\n";
    cout << "Rejected : " << rejected << "\n";
}

// ---------------------- Chatbot Loop (with IoU conversation) -------
void startBot(string triggers[], string responses[], int count) {
    // load human chat corpus for IoU general conversation
    auto corpus = loadHumanCorpus("human_chat_corpus.txt");

    if (count == 0) {
        cout << "Bot: No utterances loaded.\n";
    }

    string defaultResponse = "Sorry, I don't understand.";
    for (int i = 0; i < count; ++i)
        if (triggers[i] == "*")
            defaultResponse = responses[i];

    cout << "Welcome to Loan Processing Chatbot!  Hello! I can help you submit a loan application, show loan plans, or check your application status.\n(Type X to exit)\n";

    // Command menu
    auto showMenu = []() {
        cout << "Welcome to Loan Processing Chatbot!  Hello! I can help you submit a loan application, show loan plans, or check your application status.\n(Type X to exit)\n";
        cout << "\n================ AVAILABLE COMMANDS ================\n";
        cout << "A       - Start/Resume multi-session loan application\n";
        cout << "H       - View Home Loan Plans\n";
        cout << "C       - View Car Loan Plans\n";
        cout << "S       - View Scooter Loan Plans\n";
        cout << "P       - View Personal Loan Plans\n";
        cout << "STATUS  - Check status by Application ID\n";
        cout << "COUNT   - See Submitted/Approved/Rejected counts by CNIC\n";
        cout << "CHAT    - Enter general conversation mode (IoU retrieval)\n";
        cout << "HELP    - Show this command menu again\n";
        cout << "X       - Exit the chatbot\n";
        cout << "====================================================\n\n";
        };

    showMenu();

    while (true) {
        cout << "You: ";
        string input;
        getline(cin, input);
        if (input.empty()) { cout << "Bot: Please type something.\n"; continue; }
        string u = toUpperCase(input);

        // Exit
        if (u == "X") { cout << "Bot: Goodbye!\n"; break; }

        // HELP
        if (u == "HELP") { showMenu(); continue; }

        // Built-in commands
        if (u == "A") {
            cout << "Bot: Starting/resuming multi-session application.\n";
            processMultiSessionApplication();
            continue;
        }
        if (u == "H") { showHomeLoanPlans(); continue; }
        if (u == "C") { showCarLoanPlans(); continue; }
        if (u == "S") { showScooterLoanPlans(); continue; }
        if (u == "P") { showPersonalLoanPlans(); continue; }
        if (u == "STATUS" || u == "CHECK") {
            cout << "Enter Application ID: ";
            string id; getline(cin, id);
            checkApplicationStatus(id);
            continue;
        }
        if (u == "COUNT") {
            cout << "Enter CNIC (no dashes): ";
            string cnic; getline(cin, cnic);
            countApplicationsByCNIC(cnic);
            continue;
        }
        if (u == "CHAT") {
            cout << "Entering general conversation mode. (type MENU to exit chat)\n";
            while (true) {
                cout << "You: ";
                string user; getline(cin, user);
                if (toUpperCase(user) == "MENU") { cout << "Exiting general conversation mode.\n"; break; }
                if (user.empty()) { cout << "Bot: Say something.\n"; continue; }
                // 1) exact utterances match first
                bool matched = false;
                string tu = toUpperCase(user);
                for (int i = 0; i < count; ++i) if (tu == triggers[i]) { cout << "Bot: " << responses[i] << "\n"; matched = true; break; }
                if (matched) continue;
                // else IoU-based retrieval from corpus
                if (!corpus.empty()) {
                    string resp = bestIoUResponse(user, corpus);
                    cout << "Bot: " << resp << "\n";
                }
                else {
                    cout << "Bot: " << defaultResponse << "\n";
                }
            }
            continue;
        }

        // Try utterances file equality match
        bool matched = false;
        for (int i = 0; i < count; ++i) {
            if (u == triggers[i]) {
                cout << "Bot: " << responses[i] << endl;
                matched = true;
                break;
            }
        }

        if (!matched) {
            // fallback to corpus IoU (single-turn)
            if (!corpus.empty()) {
                string resp = bestIoUResponse(input, corpus);
                cout << "Bot: " << resp << endl;
            }
            else {
                cout << "Bot: " << defaultResponse << endl;
            }
        }
    }
}
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

// Helper to convert any input to uppercase for case-insensitive matching
string toUpperCase(string text) {
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] >= 'a' && text[i] <= 'z')
            text[i] -= 32;
    }
    return text;
}

void loadResponses(string triggers[], string responses[], int& count) {
    ifstream file("Utterances.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open Utterances.txt\n";
        count = 0;
        return;
    }

    string line;
    count = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        int pos = line.find('#');
        if (pos == -1) {
            cout << "Warning: Malformed line in Utterances.txt -> " << line << endl;
            continue;
        }

        if (count < 100) {
            string trigger = line.substr(0, pos);
            string response = line.substr(pos + 1);

            triggers[count] = toUpperCase(trigger);
            responses[count] = response;
            count++;
        }
        else {
            cout << "Warning: Too many entries in Utterances.txt (max 100)\n";
            break;
        }
    }
    file.close();

    if (count == 0)
        cout << "Warning: No valid entries found in Utterances.txt\n";
}

void showHomeLoanPlans() {
    ifstream file("Home.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open Home.txt (file missing or locked)\n";
        return;
    }

    string line;
    bool skipHeader = true;
    bool foundData = false;
    int lineNumber = 0;

    // Arrays to store data temporarily 
    string areas[50], sizes[50], installments[50], prices[50], downPayments[50];
    int totalPlans = 0;

    cout << "\n---------------- Home Loan Plans ----------------\n";
    cout << "Options are as follows:\n";

    while (getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        if (skipHeader) { 
            skipHeader = false;
            continue;
        }

        int pos1 = line.find('#');
        int pos2 = line.find('#', pos1 + 1);
        int pos3 = line.find('#', pos2 + 1);
        int pos4 = line.find('#', pos3 + 1);

        if (pos1 == -1 || pos2 == -1 || pos3 == -1 || pos4 == -1) {
            cout << "Warning: Line " << lineNumber << " in Home.txt is malformed.\n";
            continue;
        }

        if (totalPlans < 50) {
            areas[totalPlans] = line.substr(0, pos1);
            sizes[totalPlans] = line.substr(pos1 + 1, pos2 - pos1 - 1);
            installments[totalPlans] = line.substr(pos2 + 1, pos3 - pos2 - 1);
            prices[totalPlans] = line.substr(pos3 + 1, pos4 - pos3 - 1);
            downPayments[totalPlans] = line.substr(pos4 + 1);

            cout << totalPlans + 1 << ". "
                << "Area: " << areas[totalPlans]
                << " | Size: " << sizes[totalPlans]
                << " | Installments: " << installments[totalPlans]
                << " | Price: " << prices[totalPlans]
                << " | Down Payment: " << downPayments[totalPlans]
                << endl;

            totalPlans++;
            foundData = true;
        }
    }

    file.close();

    if (!foundData) {
        cout << "No valid loan plans found in Home.txt.\n";
        cout << "--------------------------------------------------\n\n";
        return;
    }

    cout << "--------------------------------------------------\n";

    // Get user selection
    string choice;
    int selected = -1;

    cout << "\nEnter the number of the loan plan you want to select: ";
    getline(cin, choice);

    // Validate user selection
    if (choice.length() == 1 && choice[0] >= '1' && choice[0] <= ('0' + totalPlans)) {
        selected = choice[0] - '1';
    }
    else {
        cout << "Invalid selection. Returning to main chat.\n";
        cout << "--------------------------------------------------\n\n";
        return;
    }

    cout << "\nYou selected Plan #" << (selected + 1) << ":\n";
    cout << "Area: " << areas[selected]
        << " | Size: " << sizes[selected]
        << " | Installments: " << installments[selected]
        << " | Price: " << prices[selected]
        << " | Down Payment: " << downPayments[selected] << endl;

    double price = 0, down = 0;
    int totalInst = 0;

    // Removing commas from prices 

    string p = prices[selected];
    string d = downPayments[selected];
    string inst = installments[selected];

    string cleanP = "", cleanD = "", cleanI = "";
    for (int i = 0; i < p.length(); i++) if (p[i] != ',') cleanP += p[i];
    for (int i = 0; i < d.length(); i++) if (d[i] != ',') cleanD += d[i];
    for (int i = 0; i < inst.length(); i++) if (inst[i] != ',') cleanI += inst[i];

    price = stod(cleanP);
    down = stod(cleanD);
    totalInst = stoi(cleanI);

    if (totalInst == 0) {
        cout << "Error: Installments cannot be zero.\n";
        return;
    }

    double remaining = price - down;
    double perMonth = remaining / totalInst;
    cout<<fixed<<setprecision(0);

    cout << "\nLoan Breakdown:\n";
    cout << "Total Price: " << price << " PKR\n";
    cout << "Down Payment: " << down << " PKR\n";
    cout << "Remaining Amount: " << remaining << " PKR\n";
    cout << "Installment Plan: " << totalInst << " months\n";
    cout << "Monthly Payment: " << perMonth << " PKR\n";
    cout << "--------------------------------------------------\n\n";
}


void startBot(string triggers[], string responses[], int count) {
    if (count == 0) {
        cout << "Bot: Unable to start — no utterances loaded.\n";
        return;
    }

    // Default response for any out scope input
    string defaultResponse = "Bot: Sorry, I didn’t understand.";
    for (int i = 0; i < count; i++) {
        if (triggers[i] == "*") {
            defaultResponse = responses[i];
            break;
        }
    }

    cout << "--------------------------------------\n";
    cout << "   Loan Chatbot Initialized (C++)\n";
    cout << "--------------------------------------\n";
    cout << "Type 'X' to exit.\n\n";

    string input;

    while (true) {
        cout << "You: ";
        getline(cin, input);

        // Handles empty input (straight up enter pressed)
        if (input.empty()) {
            cout << "Bot: Please enter something.\n";
            continue;
        }

        // Exit case
        if (input == "X" || input == "x") {
            cout << "Bot: Goodbye!\n";
            break;
        }

        // Convert user input to uppercase for comparison
        string upperInput = toUpperCase(input);

        // Search for matching trigger 
        bool matched = false;
        for (int i = 0; i < count; i++) {
            if (upperInput == triggers[i]) {
                cout << "Bot: " << responses[i] << endl;
                matched = true;

                // Special case: Home loan
                if (upperInput == "H")
                    showHomeLoanPlans();
                break;
                // other special cases to be added later on as the project goes forawrd
            }
        }
        // if no match found from utterances, print the default response
        if (!matched) {
            cout << "Bot: " << defaultResponse << endl;
        }
    }
}

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

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
        // Skip empty lines
        if (line.empty()) continue;

        int pos = line.find('#');
        if (pos == -1) {
            cout << "Warning: Malformed line in Utterances.txt -> " << line << endl;
            continue;
        }

        if (count < 100) {
            triggers[count] = line.substr(0, pos);
            responses[count] = line.substr(pos + 1);
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

    cout << "\n---------------- Home Loan Plans ----------------\n";

    while (getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        if (skipHeader) { // skip first line
            skipHeader = false;
            continue;
        }

        int pos1 = line.find('#');
        if (pos1 == -1) {
            cout << "Warning: Line " << lineNumber << " in Home.txt is malformed.\n";
            continue;
        }

        string area, size, installments, price, downPayment;
        int pos2 = line.find('#', pos1 + 1);
        int pos3 = line.find('#', pos2 + 1);
        int pos4 = line.find('#', pos3 + 1);

        if (pos4 == -1) {
            cout << "Warning: Missing data in line " << lineNumber << ".\n";
            continue;
        }

        area = line.substr(0, pos1);
        size = line.substr(pos1 + 1, pos2 - pos1 - 1);
        installments = line.substr(pos2 + 1, pos3 - pos2 - 1);
        price = line.substr(pos3 + 1, pos4 - pos3 - 1);
        downPayment = line.substr(pos4 + 1);

        cout << "Area: " << area
            << " | Size: " << size
            << " | Installments: " << installments
            << " | Price: " << price
            << " | Down Payment: " << downPayment << endl;

        foundData = true;
    }

    file.close();

    if (!foundData)
        cout << "No valid loan plans found in Home.txt.\n";

    cout << "--------------------------------------------------\n\n";
}


void startBot(string triggers[], string responses[], int count) {
    if (count == 0) {
        cout << "Bot: Unable to start — no utterances loaded.\n";
        return;
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

        // Search for matching trigger (H,C,P etc)
        bool matched = false;
        for (int i = 0; i < count; i++) {
            if (input == triggers[i]) {
                cout << "Bot: " << responses[i] << endl;
                matched = true;

                // For Home Loans
                if (input == "H")
                    showHomeLoanPlans();
                break;
            }
        }

        if (!matched)
            cout << "Bot: Sorry, I didn't understand that. Try again.\n";
    }

}

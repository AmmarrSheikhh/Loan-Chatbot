#include "lender.h"
#include "shared_file_utils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

// Define the shared data path
const string SHARED_DATA_PATH = "C:\\Users\\mahru\\source\\repos\\shared_data\\";

// ----------------------------------------------------
// Load all applications
// ----------------------------------------------------
vector<LenderApplication> loadAllApplications() {
    vector<LenderApplication> apps;

    string filePath = SHARED_DATA_PATH + "applications.txt";
    ifstream file(filePath);
    string line;

    if (!file.is_open()) {
        cout << "Error: Could not open applications file at: " << filePath << "\n";
        releaseLock("applications.txt");
        return apps;
    }

    while (getline(file, line)) {
        if (line.empty()) continue;

        LenderApplication app;
        string tmp;
        stringstream ss(line);

        while (getline(ss, tmp, '#')) {
            app.fields.push_back(tmp);
        }

        apps.push_back(app);
    }

    file.close();
    releaseLock("applications.txt");
    return apps;
}

// ----------------------------------------------------
// Show list of applications - FIXED DUPLICATION ISSUE
// ----------------------------------------------------
void showAllApplications() {
    auto apps = loadAllApplications();
    cout << "\n===== APPLICATION LIST =====\n";

    if (apps.empty()) {
        cout << "No applications found.\n";
        return;
    }

    // Header with formatted columns
    cout << left << setw(8) << "ID"
        << setw(20) << "Name"
        << setw(12) << "Loan Type"
        << setw(30) << "Loan Plan Details"
        << setw(12) << "Status" << endl;
    cout << string(82, '-') << endl;

    for (auto& app : apps) {
        if (app.fields.size() < 2) continue;

        string name = app.fields[0];
        string appID = "Unknown";
        string status = "Unknown";
        string loanType = "Unknown";
        string loanPlan = "No plan selected";

        // Get application ID (field 36, index 35)
        if (app.fields.size() > 35) {
            appID = app.fields[35];
        }

        // Get status - it should be the LAST field
        if (!app.fields.empty()) {
            status = app.fields.back();
        }

        // Get loan type (field 38, index 37)
        if (app.fields.size() > 37) {
            loanType = app.fields[37];
            // If loan type contains plan details, extract just the type
            if (loanType.find("Area") != string::npos || loanType.find("Marla") != string::npos) {
                // This is actually a loan plan, not a type - reset it
                loanType = "HOME";
            }
        }

        // Get loan plan details (field 39, index 38)
        if (app.fields.size() > 38) {
            loanPlan = app.fields[38];

            // FIX: If loan plan contains the status, extract just the plan part
            size_t statusPos = loanPlan.find("Approved");
            if (statusPos == string::npos) statusPos = loanPlan.find("Rejected");
            if (statusPos == string::npos) statusPos = loanPlan.find("Submitted");

            if (statusPos != string::npos) {
                // Extract only the loan plan part (before the status)
                loanPlan = loanPlan.substr(0, statusPos);
                // Remove trailing spaces/dashes
                while (!loanPlan.empty() && (loanPlan.back() == ' ' || loanPlan.back() == '-' || loanPlan.back() == '|')) {
                    loanPlan.pop_back();
                }
            }

            // FIX: If loan plan is actually a status, reset it
            if (loanPlan == "Approved" || loanPlan == "Rejected" || loanPlan == "Submitted") {
                loanPlan = "No plan selected";
            }
        }

        // SPECIAL FIX: If loan plan and status are the same, reset loan plan
        if (loanPlan == status && loanPlan != "No plan selected") {
            loanPlan = "No plan selected";
        }

        // FIX: If we have a loan type but no plan, and the status contains plan details
        if (loanType != "Unknown" && loanPlan == "No plan selected" &&
            (status.find("Area") != string::npos || status.find("Marla") != string::npos || status.find("PKR") != string::npos)) {
            loanPlan = status;
            status = "Submitted"; // Default status
        }

        // Truncate for display
        string displayName = name.length() > 18 ? name.substr(0, 15) + "..." : name;
        string displayPlan = loanPlan.length() > 27 ? loanPlan.substr(0, 24) + "..." : loanPlan;

        cout << left << setw(8) << appID
            << setw(20) << displayName
            << setw(12) << loanType
            << setw(30) << displayPlan
            << setw(12) << status << endl;
    }
}

// ----------------------------------------------------
// Show details of a specific application
// ----------------------------------------------------
void showApplicationDetails(const LenderApplication& app) {
    cout << "\n===== APPLICATION DETAILS =====\n";

    // DEBUG: Show field count
    cout << "Total fields: " << app.fields.size() << endl;

    vector<string> fieldNames = {
        "Full Name", "Father's Name", "Postal Address", "Contact Number",
        "Email", "CNIC", "CNIC Expiry", "Employment Status", "Marital Status",
        "Gender", "Dependents", "Annual Income", "Avg Electricity Bill",
        "Current Electric Bill", "Has Previous Loan", "Previous Loan Active",
        "Previous Loan Total", "Previous Loan Returned", "Previous Loan Due",
        "Previous Loan Bank", "Previous Loan Category", "Reference 1 Name",
        "Reference 1 CNIC", "Reference 1 Issue Date", "Reference 1 Phone",
        "Reference 1 Email", "Reference 2 Name", "Reference 2 CNIC",
        "Reference 2 Issue Date", "Reference 2 Phone", "Reference 2 Email",
        "CNIC Front Image", "CNIC Back Image", "Bill Image", "Salary Image",
        "Application ID", "Status", "Loan Type", "Loan Plan Details"
    };

    for (int i = 0; i < (int)app.fields.size() && i < (int)fieldNames.size(); i++) {
        cout << fieldNames[i] << ": " << app.fields[i] << endl;
    }

    // Show actual field indices for debugging
    cout << "\n--- RAW FIELD INDICES ---" << endl;
    for (int i = 0; i < app.fields.size(); i++) {
        cout << "[" << i << "]: " << app.fields[i] << endl;
    }
}

// ----------------------------------------------------
// Find app index in vector by ID
// ----------------------------------------------------
int findApplicationIndex(const vector<LenderApplication>& apps, const string& appID) {
    for (int i = 0; i < (int)apps.size(); i++) {
        if (apps[i].fields.size() > 35) {
            if (apps[i].fields[35] == appID)
                return i;
        }
    }
    return -1;
}

// ----------------------------------------------------
// Approve or Reject application - FIXED
// ----------------------------------------------------
void updateApplicationStatus(const string& appID, const string& newStatus) {
    if (!acquireLock("applications.txt")) {
        cout << "Error: Could not access applications file. It might be in use.\n";
        return;
    }

    auto apps = loadAllApplications();
    int idx = findApplicationIndex(apps, appID);

    if (idx == -1) {
        cout << "Invalid Application ID.\n";
        releaseLock("applications.txt");
        return;
    }

    // FIX: Always update the LAST field as status
    if (!apps[idx].fields.empty()) {
        apps[idx].fields.back() = newStatus;
    }

    // Rewrite file
    string filePath = SHARED_DATA_PATH + "applications.txt";
    ofstream out(filePath);
    if (!out.is_open()) {
        cout << "Error: Could not save changes to applications file.\n";
        releaseLock("applications.txt");
        return;
    }

    for (auto& app : apps) {
        for (int i = 0; i < (int)app.fields.size(); i++) {
            out << app.fields[i];
            if (i != (int)app.fields.size() - 1) out << "#";
        }
        out << "\n";
    }
    out.close();

    cout << "\nApplication " << appID << " has been " << newStatus << "\n";

    releaseLock("applications.txt");
}

// ----------------------------------------------------
// View application details by ID
// ----------------------------------------------------
void viewApplicationDetails() {
    cout << "Enter Application ID: ";
    string id;
    getline(cin, id);

    auto apps = loadAllApplications();
    int idx = findApplicationIndex(apps, id);

    if (idx == -1) {
        cout << "Invalid Application ID.\n";
        return;
    }

    showApplicationDetails(apps[idx]);
}

// ----------------------------------------------------
// Generate monthly payment plan
// ----------------------------------------------------
void generateMonthlyPlanForLender() {
    cout << "Enter Application ID to view payment plan: ";
    string appID;
    getline(cin, appID);

    auto apps = loadAllApplications();
    int idx = findApplicationIndex(apps, appID);

    if (idx == -1) {
        cout << "Invalid Application ID.\n";
        return;
    }

    string loanPlan = "No plan selected";

    // Get loan plan from field 39 (index 38)
    if (apps[idx].fields.size() > 38) {
        loanPlan = apps[idx].fields[38];
    }

    cout << "\n=== PAYMENT PLAN FOR APPLICATION " << appID << " ===\n";
    if (loanPlan == "No plan selected" || loanPlan.empty()) {
        cout << "No payment plan available for this application.\n";
    }
    else {
        cout << "Loan Plan: " << loanPlan << "\n";
    }
    cout << "=========================\n";
}
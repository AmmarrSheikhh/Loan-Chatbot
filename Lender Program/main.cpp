#include <iostream>
#include "lender.h"

using namespace std;

int main() {
    cout << "===================================\n";
    cout << "      LENDER DASHBOARD SYSTEM     \n";
    cout << "===================================\n";

    while (true) {
        cout << "\n===== LENDER MENU =====\n";
        cout << "1. View all applications\n";
        cout << "2. View application details\n";
        cout << "3. Approve an application\n";
        cout << "4. Reject an application\n";
        cout << "5. Generate monthly payment plan\n";
        cout << "6. Exit\n";
        cout << "Select option: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            showAllApplications();
        }
        else if (choice == "2") {
            cout << "Enter Application ID: ";
            string id;
            getline(cin, id);

            auto apps = loadAllApplications();
            int idx = findApplicationIndex(apps, id);

            if (idx == -1) {
                cout << "Invalid Application ID.\n";
            }
            else {
                showApplicationDetails(apps[idx]);
            }
        }
        else if (choice == "3") {
            showAllApplications();
            cout << "Enter Application ID to APPROVE: ";
            string id;
            getline(cin, id);
            updateApplicationStatus(id, "Approved");
        }
        else if (choice == "4") {
            showAllApplications();
            cout << "Enter Application ID to REJECT: ";
            string id;
            getline(cin, id);
            updateApplicationStatus(id, "Rejected");
        }
        else if (choice == "5") {
            generateMonthlyPlanForLender();
        }
        else if (choice == "6") {
            cout << "Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid option. Please try again.\n";
        }
    }

    return 0;
}
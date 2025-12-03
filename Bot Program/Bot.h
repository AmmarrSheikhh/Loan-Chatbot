#ifndef BOT_H
#define BOT_H

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

string toUpperCase(string text);
void loadResponses(string triggers[], string responses[], int& count);

void showHomeLoanPlans();
void showCarLoanPlans();
void showScooterLoanPlans();
void showPersonalLoanPlans();

void copyImageToDataFolder(const string& srcPath, const string& destFolder);

struct ApplicationNode {
    string fullName;
    string fatherName;
    string postalAddress;
    string contactNumber;
    string email;
    string cnic;
    string cnicExpiry;
    string employmentStatus;
    string maritalStatus;
    string gender;
    string dependents;
    string annualIncome;
    string avgElectricityBill;
    string currentElectricBill;

    string hasPrevLoan;
    string prevLoanActive;
    string prevLoanTotal;
    string prevLoanReturned;
    string prevLoanDue;
    string prevLoanBank;
    string prevLoanCategory;

    string ref1Name;
    string ref1CNIC;
    string ref1Issue;
    string ref1Phone;
    string ref1Email;

    string ref2Name;
    string ref2CNIC;
    string ref2Issue;
    string ref2Phone;
    string ref2Email;

    string cnicFrontImg;
    string cnicBackImg;
    string billImg;
    string salaryImg;

    string applicationID;
    string status;
    string loanType;        // HOME, CAR, SCOOTER, PERSONAL
    string loanPlanDetails; // Specific plan details (e.g., "Area 1 - 5 Marla - 10,000,000 PKR")

    ApplicationNode* next;
};
extern ApplicationNode* head;

ApplicationNode* createApplicationNode();
void insertNode(ApplicationNode* node);
string generateAppID();
void saveApplicationToFile(ApplicationNode* node);
void processFullApplication();
void processMultiSessionApplication();

void checkApplicationStatus(const string& appID);
void countApplicationsByCNIC(const string& cnic);

void startBot(string triggers[], string responses[], int count);

#endif
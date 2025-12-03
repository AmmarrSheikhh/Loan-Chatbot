#ifndef LENDER_H
#define LENDER_H

#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct LenderApplication {
    vector<string> fields;   // each field from applications.txt
};

// Load all applications
vector<LenderApplication> loadAllApplications();

// Display short list
void showAllApplications();

// Display full details of one application
void showApplicationDetails(const LenderApplication& app);

// Approve / Reject
void updateApplicationStatus(const string& appID, const string& newStatus);

// Monthly plan generator (used only on approval)
void generateMonthlyPlanForLender();

// Find application by ID
int findApplicationIndex(const vector<LenderApplication>& apps, const string& appID);

#endif
#include "shared_file_utils.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std;

// Define the shared data path (must match the one in lender_functions.cpp)
const string SHARED_DATA_PATH = "C:\\Users\\mahru\\source\\repos\\shared_data\\";

bool acquireLock(const std::string& filename) {
    string lockFile = SHARED_DATA_PATH + filename + ".lock";
    int attempts = 0;

    while (attempts < 10) {
        ifstream test(lockFile);
        if (!test.is_open()) {
            // Create lock file
            ofstream lock(lockFile);
            if (lock.is_open()) {
                lock << "locked";
                lock.close();
                return true;
            }
        }
        test.close();
        this_thread::sleep_for(chrono::milliseconds(100));
        attempts++;
    }
    cout << "Warning: File is busy, waiting...\n";
    return false;
}

void releaseLock(const std::string& filename) {
    string lockFile = SHARED_DATA_PATH + filename + ".lock";
    remove(lockFile.c_str());
}
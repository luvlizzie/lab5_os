//
//  employee.cpp
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#include "../include/employee.h"
#include <iostream>
#include <algorithm>
#include <cstring>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ofstream;
using std::ifstream;
using std::ios;

EmployeeFile::EmployeeFile(const string& name) : filename(name) {
#ifdef _WIN32
    InitializeCriticalSection(&lockMutex);
#else
    pthread_mutex_init(&lockMutex, NULL);
#endif
}

EmployeeFile::~EmployeeFile() {
    close();
#ifdef _WIN32
    DeleteCriticalSection(&lockMutex);
#else
    pthread_mutex_destroy(&lockMutex);
#endif
}

bool EmployeeFile::create(const vector<Employee>& employees) {
    ofstream outFile(filename, ios::binary);
    if (!outFile.is_open()) {
        return false;
    }
    
    for (const auto& emp : employees) {
        outFile.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    }
    
    outFile.close();
    
    readLocks.assign(employees.size(), false);
    writeLocks.assign(employees.size(), false);
    lockOwners.assign(employees.size(), -1);
    
    return true;
}

bool EmployeeFile::open() {
    file.open(filename, ios::binary | ios::in | ios::out);
    return file.is_open();
}

void EmployeeFile::close() {
    if (file.is_open()) {
        file.close();
    }
}

bool EmployeeFile::readRecord(int id, Employee& emp) {
    if (!file.is_open()) return false;
    
    // Calculate position (id is 1-based)
    file.seekg((id - 1) * sizeof(Employee), ios::beg);
    file.read(reinterpret_cast<char*>(&emp), sizeof(Employee));
    
    return file.good();
}

bool EmployeeFile::writeRecord(int id, const Employee& emp) {
    if (!file.is_open()) return false;
    
    file.seekp((id - 1) * sizeof(Employee), ios::beg);
    file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    file.flush();
    
    return file.good();
}

bool EmployeeFile::recordExists(int id) {
    if (!file.is_open()) return false;
    
    file.seekg((id - 1) * sizeof(Employee), ios::beg);
    Employee temp;
    file.read(reinterpret_cast<char*>(&temp), sizeof(Employee));
    
    return file.good() && temp.num != 0;
}

bool EmployeeFile::lockRecord(int id, bool exclusive) {
#ifdef _WIN32
    EnterCriticalSection(&lockMutex);
#else
    pthread_mutex_lock(&lockMutex);
#endif
    
    int idx = id - 1;
    bool success = false;
    
    if (exclusive) {
        // Exclusive lock (for write)
        if (!writeLocks[idx] && !readLocks[idx]) {
            writeLocks[idx] = true;
            lockOwners[idx] = 1; // Client ID placeholder
            success = true;
        }
    } else {
        // Shared lock (for read)
        if (!writeLocks[idx]) {
            readLocks[idx] = true;
            lockOwners[idx] = 1;
            success = true;
        }
    }
    
#ifdef _WIN32
    LeaveCriticalSection(&lockMutex);
#else
    pthread_mutex_unlock(&lockMutex);
#endif
    
    return success;
}

bool EmployeeFile::unlockRecord(int id) {
#ifdef _WIN32
    EnterCriticalSection(&lockMutex);
#else
    pthread_mutex_lock(&lockMutex);
#endif
    
    int idx = id - 1;
    
    if (writeLocks[idx]) {
        writeLocks[idx] = false;
    } else if (readLocks[idx]) {
        readLocks[idx] = false;
    }
    
    lockOwners[idx] = -1;
    
#ifdef _WIN32
    LeaveCriticalSection(&lockMutex);
#else
    pthread_mutex_unlock(&lockMutex);
#endif
    
    return true;
}

void EmployeeFile::printAll() {
    if (!file.is_open()) return;
    
    cout << "\n=== Employee Records ===" << endl;
    cout << "ID\tName\t\tHours" << endl;
    cout << "===========================" << endl;
    
    file.seekg(0, ios::beg);
    Employee emp;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        if (emp.num != 0) {
            cout << emp.num << "\t" << emp.name << "\t\t" << emp.hours << endl;
        }
    }
    cout << "===========================" << endl;
    
    file.clear(); // Clear EOF flag
}

int EmployeeFile::getRecordCount() {
    if (!file.is_open()) return 0;
    
    file.seekg(0, ios::end);
    int size = file.tellg();
    file.seekg(0, ios::beg);
    
    return size / sizeof(Employee);
}

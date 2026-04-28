//
//  employee.h
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <vector>
#include <fstream>
#include "../include/common.h"

class EmployeeFile {
private:
    std::string filename;
    std::fstream file;
    
public:
    EmployeeFile(const std::string& name);
    ~EmployeeFile();
    
    bool create(const std::vector<Employee>& employees);
    bool open();
    void close();
    
    bool readRecord(int id, Employee& emp);
    bool writeRecord(int id, const Employee& emp);
    bool recordExists(int id);
    
    void printAll();
    int getRecordCount();
    
    // Lock management
    bool lockRecord(int id, bool exclusive);
    bool unlockRecord(int id);
    
private:
    std::vector<bool> readLocks;
    std::vector<bool> writeLocks;
    std::vector<int> lockOwners;
    
    #ifdef _WIN32
    CRITICAL_SECTION lockMutex;
    #else
    pthread_mutex_t lockMutex;
    #endif
};

#endif

//
//  test_employee.cpp
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdio>
#include <cstring>
#include "../include/employee.h"

using ::testing::Eq;
using ::testing::DoubleEq;

class EmployeeFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        system("mkdir -p test_data");
        filename = "test_data/test_employees.dat";
    }
    
    void TearDown() override {
        system("rm -rf test_data");
    }
    
    std::string filename;
};

TEST_F(EmployeeFileTest, CreatesFileSuccessfully) {
    std::vector<Employee> employees;
    
    Employee emp1;
    emp1.num = 1;
    strncpy(emp1.name, "John", MAX_NAME_LEN - 1);
    emp1.name[MAX_NAME_LEN - 1] = '\0';
    emp1.hours = 160.0;
    
    Employee emp2;
    emp2.num = 2;
    strncpy(emp2.name, "Jane", MAX_NAME_LEN - 1);
    emp2.name[MAX_NAME_LEN - 1] = '\0';
    emp2.hours = 155.0;
    
    employees.push_back(emp1);
    employees.push_back(emp2);
    
    EmployeeFile empFile(filename);
    bool result = empFile.create(employees);
    
    EXPECT_TRUE(result);
    
    // Check file exists
    std::ifstream file(filename);
    EXPECT_TRUE(file.good());
}

TEST_F(EmployeeFileTest, ReadsRecordCorrectly) {
    std::vector<Employee> employees;
    
    Employee emp1;
    emp1.num = 1;
    strncpy(emp1.name, "John", MAX_NAME_LEN - 1);
    emp1.name[MAX_NAME_LEN - 1] = '\0';
    emp1.hours = 160.0;
    
    employees.push_back(emp1);
    
    EmployeeFile empFile(filename);
    empFile.create(employees);
    empFile.open();
    
    Employee readEmp;
    bool result = empFile.readRecord(1, readEmp);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(readEmp.num, 1);
    EXPECT_STREQ(readEmp.name, "John");
    EXPECT_DOUBLE_EQ(readEmp.hours, 160.0);
}

TEST_F(EmployeeFileTest, WritesRecordCorrectly) {
    std::vector<Employee> employees;
    
    Employee emp1;
    emp1.num = 1;
    strncpy(emp1.name, "John", MAX_NAME_LEN - 1);
    emp1.name[MAX_NAME_LEN - 1] = '\0';
    emp1.hours = 160.0;
    
    employees.push_back(emp1);
    
    EmployeeFile empFile(filename);
    empFile.create(employees);
    empFile.open();
    
    Employee modifiedEmp;
    modifiedEmp.num = 1;
    strncpy(modifiedEmp.name, "Modified", MAX_NAME_LEN - 1);
    modifiedEmp.name[MAX_NAME_LEN - 1] = '\0';
    modifiedEmp.hours = 175.0;
    
    bool result = empFile.writeRecord(1, modifiedEmp);
    EXPECT_TRUE(result);
    
    Employee readEmp;
    empFile.readRecord(1, readEmp);
    EXPECT_STREQ(readEmp.name, "Modified");
    EXPECT_DOUBLE_EQ(readEmp.hours, 175.0);
}

TEST_F(EmployeeFileTest, LockUnlockRecordWorks) {
    std::vector<Employee> employees;
    
    Employee emp1;
    emp1.num = 1;
    strncpy(emp1.name, "John", MAX_NAME_LEN - 1);
    emp1.name[MAX_NAME_LEN - 1] = '\0';
    emp1.hours = 160.0;
    
    employees.push_back(emp1);
    
    EmployeeFile empFile(filename);
    empFile.create(employees);
    
    // Acquire exclusive lock
    bool lockResult = empFile.lockRecord(1, true);
    EXPECT_TRUE(lockResult);
    
    // Try to acquire another exclusive lock (should fail)
    bool secondLock = empFile.lockRecord(1, true);
    EXPECT_FALSE(secondLock);
    
    // Release lock
    empFile.unlockRecord(1);
    
    // Lock again (should succeed)
    lockResult = empFile.lockRecord(1, true);
    EXPECT_TRUE(lockResult);
    
    empFile.unlockRecord(1);
}

TEST_F(EmployeeFileTest, SharedLocksAllowMultipleReaders) {
    std::vector<Employee> employees;
    
    Employee emp1;
    emp1.num = 1;
    strncpy(emp1.name, "John", MAX_NAME_LEN - 1);
    emp1.name[MAX_NAME_LEN - 1] = '\0';
    emp1.hours = 160.0;
    
    employees.push_back(emp1);
    
    EmployeeFile empFile(filename);
    empFile.create(employees);
    
    // Acquire first shared lock
    bool lock1 = empFile.lockRecord(1, false);
    EXPECT_TRUE(lock1);
    
    // Acquire second shared lock (should succeed)
    bool lock2 = empFile.lockRecord(1, false);
    EXPECT_TRUE(lock2);
    
    // Release locks
    empFile.unlockRecord(1);
    empFile.unlockRecord(1);
}

TEST_F(EmployeeFileTest, ExclusiveLockPreventsSharedLock) {
    std::vector<Employee> employees;
    
    Employee emp1;
    emp1.num = 1;
    strncpy(emp1.name, "John", MAX_NAME_LEN - 1);
    emp1.name[MAX_NAME_LEN - 1] = '\0';
    emp1.hours = 160.0;
    
    employees.push_back(emp1);
    
    EmployeeFile empFile(filename);
    empFile.create(employees);
    
    // Acquire exclusive lock
    bool exclusiveLock = empFile.lockRecord(1, true);
    EXPECT_TRUE(exclusiveLock);
    
    // Try to acquire shared lock (should fail)
    bool sharedLock = empFile.lockRecord(1, false);
    EXPECT_FALSE(sharedLock);
    
    empFile.unlockRecord(1);
}

TEST_F(EmployeeFileTest, RecordExists) {
    std::vector<Employee> employees;
    
    Employee emp1;
    emp1.num = 1;
    strncpy(emp1.name, "John", MAX_NAME_LEN - 1);
    emp1.name[MAX_NAME_LEN - 1] = '\0';
    emp1.hours = 160.0;
    
    Employee emp2;
    emp2.num = 2;
    strncpy(emp2.name, "Jane", MAX_NAME_LEN - 1);
    emp2.name[MAX_NAME_LEN - 1] = '\0';
    emp2.hours = 155.0;
    
    employees.push_back(emp1);
    employees.push_back(emp2);
    
    EmployeeFile empFile(filename);
    empFile.create(employees);
    empFile.open();
    
    EXPECT_TRUE(empFile.recordExists(1));
    EXPECT_TRUE(empFile.recordExists(2));
    EXPECT_FALSE(empFile.recordExists(3));
}

TEST_F(EmployeeFileTest, GetRecordCount) {
    std::vector<Employee> employees;
    
    for (int i = 1; i <= 3; ++i) {
        Employee emp;
        emp.num = i;
        snprintf(emp.name, MAX_NAME_LEN, "Emp%d", i);
        emp.hours = i * 100.0;
        employees.push_back(emp);
    }
    
    EmployeeFile empFile(filename);
    empFile.create(employees);
    empFile.open();
    
    int count = empFile.getRecordCount();
    EXPECT_EQ(count, 3);
}

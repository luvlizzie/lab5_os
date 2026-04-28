//
//  server.cpp
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include "../include/common.h"
#include "../include/employee.h"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::thread;

static std::atomic<bool> serverRunning(true);
static vector<PipeHandle> clientHandles;
static vector<std::thread> clientThreads;

int getValidatedInt(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min && value <= max) {
            cin.ignore(256, '\n');
            return value;
        }
        cerr << "Invalid input. Please enter a number between "
             << min << " and " << max << endl;
        cin.clear();
        cin.ignore(256, '\n');
    }
}

double getValidatedDouble(const string& prompt, double min, double max) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min && value <= max) {
            cin.ignore(256, '\n');
            return value;
        }
        cerr << "Invalid input. Please enter a number between "
             << min << " and " << max << endl;
        cin.clear();
        cin.ignore(256, '\n');
    }
}

string getValidatedString(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        std::getline(cin, value);
        if (!value.empty()) {
            return value;
        }
        cerr << "Input cannot be empty. Please try again." << endl;
    }
}

Employee inputEmployee(int id) {
    Employee emp;
    emp.num = id;
    
    cout << "\nEmployee #" << id << endl;
    cout << "Enter name (max " << (MAX_NAME_LEN - 1) << " chars): ";
    cin.getline(emp.name, MAX_NAME_LEN);
    
    emp.hours = getValidatedDouble("Enter hours worked: ", 0, 1000);
    
    return emp;
}

void printEmployee(const Employee& emp) {
    cout << "ID: " << emp.num << endl;
    cout << "Name: " << emp.name << endl;
    cout << "Hours: " << emp.hours << endl;
}

void handleClient(PipeHandle clientPipe, int clientId, EmployeeFile* empFile) {
    consolePrint("Client " + std::to_string(clientId) + " connected\n");
    
    while (serverRunning) {
        Request request;
        if (!readFromPipe(clientPipe, &request, sizeof(Request))) {
            break;
        }
        
        Response response;
        response.op = Operation::ERROR;
        response.status = -1;
        
        switch (request.op) {
            case Operation::READ: {
                consolePrint("Client " + std::to_string(clientId) +
                            " requests READ for record " + std::to_string(request.recordId) + "\n");
                
                // Try to acquire shared lock
                if (empFile->lockRecord(request.recordId, false)) {
                    // Read the record
                    Employee emp;
                    if (empFile->readRecord(request.recordId, emp)) {
                        response.op = Operation::READ_RESPONSE;
                        response.status = 0;
                        response.employee = emp;
                        strcpy(response.message, "Record read successfully");
                    } else {
                        strcpy(response.message, "Record not found");
                    }
                    
                    writeToPipe(clientPipe, &response, sizeof(Response));
                    
                    // Wait for client to release the lock
                    Request releaseRequest;
                    readFromPipe(clientPipe, &releaseRequest, sizeof(Request));
                    
                    empFile->unlockRecord(request.recordId);
                    consolePrint("Client " + std::to_string(clientId) +
                                " released lock on record " + std::to_string(request.recordId) + "\n");
                } else {
                    strcpy(response.message, "Record is locked by another client");
                    writeToPipe(clientPipe, &response, sizeof(Response));
                }
                break;
            }
            case Operation::MODIFY: {
                consolePrint("Client " + std::to_string(clientId) +
                            " requests MODIFY for record " + std::to_string(request.recordId) + "\n");
                
                // Try to acquire exclusive lock
                if (empFile->lockRecord(request.recordId, true)) {
                    // Send current record to client
                    Employee emp;
                    if (empFile->readRecord(request.recordId, emp)) {
                        response.op = Operation::MODIFY_REQUEST;
                        response.status = 0;
                        response.employee = emp;
                        strcpy(response.message, "Record locked for modification");
                    } else {
                        strcpy(response.message, "Record not found");
                        response.status = -1;
                    }
                    
                    writeToPipe(clientPipe, &response, sizeof(Response));
                    
                    // Wait for modified record from client
                    Request modifyRequest;
                    readFromPipe(clientPipe, &modifyRequest, sizeof(Request));
                    
                    if (modifyRequest.op == Operation::MODIFY_RESPONSE) {
                        // Write modified record
                        empFile->writeRecord(request.recordId, modifyRequest.employee);
                        consolePrint("Client " + std::to_string(clientId) +
                                    " modified record " + std::to_string(request.recordId) + "\n");
                    }
                    
                    // Wait for client to release the lock
                    Request releaseRequest;
                    readFromPipe(clientPipe, &releaseRequest, sizeof(Request));
                    
                    empFile->unlockRecord(request.recordId);
                    consolePrint("Client " + std::to_string(clientId) +
                                " released lock on record " + std::to_string(request.recordId) + "\n");
                } else {
                    strcpy(response.message, "Record is locked by another client");
                    writeToPipe(clientPipe, &response, sizeof(Response));
                }
                break;
            }
            case Operation::EXIT: {
                consolePrint("Client " + std::to_string(clientId) + " disconnected\n");
                return;
            }
            default:
                break;
        }
    }
    
    closePipe(clientPipe);
}

int main() {
    try {
        cout << "\n=== Named Pipe Server - Employee Database ===" << endl;
        cout << string(50, '=') << endl;
        
        // Step 1.1: Create binary file
        string filename = getValidatedString("Enter employee file name: ");
        int recordCount = getValidatedInt("Enter number of employees: ", 1, MAX_RECORDS);
        
        vector<Employee> employees;
        for (int i = 1; i <= recordCount; ++i) {
            employees.push_back(inputEmployee(i));
        }
        
        EmployeeFile empFile(filename);
        if (!empFile.create(employees)) {
            cerr << "Error: Cannot create employee file" << endl;
            return 1;
        }
        
        consolePrint("Employee file created successfully\n");
        
        // Step 1.2: Print created file
        if (!empFile.open()) {
            cerr << "Error: Cannot open employee file" << endl;
            return 1;
        }
        empFile.printAll();
        
        // Step 1.3: Get number of clients and create named pipe
        int clientCount = getValidatedInt("Enter number of clients: ", 1, MAX_CLIENTS);
        
        string pipeName = getPipeName();
        PipeHandle pipeHandle;
        
        if (!createNamedPipe(pipeName, pipeHandle)) {
            cerr << "Error: Cannot create named pipe" << endl;
            return 1;
        }
        
        consolePrint("Named pipe created: " + pipeName + "\n");
        consolePrint("Waiting for clients to connect...\n");
        
        // Step 1.4: Accept client connections
        for (int i = 0; i < clientCount; ++i) {
            PipeHandle clientPipe;
            if (!waitForClient(pipeHandle)) {
                cerr << "Error: Failed to accept client" << endl;
                continue;
            }
            clientPipe = pipeHandle;
            clientHandles.push_back(clientPipe);
            
            clientThreads.emplace_back(handleClient, clientPipe, i + 1, &empFile);
            
            // Create new pipe for next client
            if (i < clientCount - 1) {
                if (!createNamedPipe(pipeName, pipeHandle)) {
                    cerr << "Error: Cannot create pipe for next client" << endl;
                }
            }
        }
        
        consolePrint("All clients connected. Server is running...\n");
        
        // Step 1.5: Wait for clients to finish
        for (auto& t : clientThreads) {
            if (t.joinable()) {
                t.join();
            }
        }
        
        // Step 1.6: Print modified file
        cout << "\n=== Modified Employee File ===" << endl;
        empFile.printAll();
        
        // Step 1.7: Wait for server termination command
        cout << "\nPress Enter to terminate server...";
        cin.get();
        
        serverRunning = false;
        
        // Cleanup
        for (auto handle : clientHandles) {
            closePipe(handle);
        }
        
        empFile.close();
        closePipe(pipeHandle);
        
        cout << "\n=== Server terminated ===" << endl;
        
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

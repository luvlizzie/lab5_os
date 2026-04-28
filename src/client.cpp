//
//  client.cpp
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#include <iostream>
#include <string>
#include "../include/common.h"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;

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

void printEmployee(const Employee& emp) {
    cout << "\n=== Employee Record ===" << endl;
    cout << "ID: " << emp.num << endl;
    cout << "Name: " << emp.name << endl;
    cout << "Hours: " << emp.hours << endl;
    cout << "=======================" << endl;
}

int main() {
    try {
        cout << "\n=== Named Pipe Client ===" << endl;
        cout << string(30, '=') << endl;
        
        // Connect to named pipe
        string pipeName = getPipeName();
        PipeHandle pipeHandle;
        
        consolePrint("Connecting to server...\n");
        if (!connectToPipe(pipeName, pipeHandle)) {
            cerr << "Error: Cannot connect to named pipe. Make sure server is running." << endl;
            return 1;
        }
        
        consolePrint("Connected to server successfully\n");
        
        bool running = true;
        int clientId = 0; // Will be assigned by server
        
        while (running) {
            cout << "\n--- Client Menu ---" << endl;
            cout << "1. Read record" << endl;
            cout << "2. Modify record" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter choice: ";
            
            int choice = getValidatedInt("", 1, 3);
            
            switch (choice) {
                case 1: { // Read record
                    int recordId = getValidatedInt("Enter record ID to read: ", 1, MAX_RECORDS);
                    
                    Request request;
                    request.op = Operation::READ;
                    request.recordId = recordId;
                    request.clientId = clientId;
                    
                    if (!writeToPipe(pipeHandle, &request, sizeof(Request))) {
                        cerr << "Error: Failed to send request" << endl;
                        break;
                    }
                    
                    Response response;
                    if (!readFromPipe(pipeHandle, &response, sizeof(Response))) {
                        cerr << "Error: Failed to receive response" << endl;
                        break;
                    }
                    
                    if (response.status == 0) {
                        printEmployee(response.employee);
                        cout << "Message: " << response.message << endl;
                        
                        // Release the lock
                        Request releaseRequest;
                        releaseRequest.op = Operation::EXIT;
                        writeToPipe(pipeHandle, &releaseRequest, sizeof(Request));
                    } else {
                        cerr << "Error: " << response.message << endl;
                    }
                    break;
                }
                case 2: { // Modify record
                    int recordId = getValidatedInt("Enter record ID to modify: ", 1, MAX_RECORDS);
                    
                    Request request;
                    request.op = Operation::MODIFY;
                    request.recordId = recordId;
                    request.clientId = clientId;
                    
                    if (!writeToPipe(pipeHandle, &request, sizeof(Request))) {
                        cerr << "Error: Failed to send request" << endl;
                        break;
                    }
                    
                    Response response;
                    if (!readFromPipe(pipeHandle, &response, sizeof(Response))) {
                        cerr << "Error: Failed to receive response" << endl;
                        break;
                    }
                    
                    if (response.status == 0) {
                        cout << "\nCurrent record:" << endl;
                        printEmployee(response.employee);
                        cout << "Message: " << response.message << endl;
                        
                        // Get modified data
                        Employee modifiedEmp = response.employee;
                        cout << "\nEnter new values (press Enter to keep current):" << endl;
                        
                        cout << "New name (" << modifiedEmp.name << "): ";
                        string newName;
                        std::getline(cin, newName);
                        if (!newName.empty()) {
                            strncpy(modifiedEmp.name, newName.c_str(), MAX_NAME_LEN - 1);
                            modifiedEmp.name[MAX_NAME_LEN - 1] = '\0';
                        }
                        
                        double newHours = getValidatedDouble(
                            "New hours (" + std::to_string(modifiedEmp.hours) + "): ", 0, 1000);
                        modifiedEmp.hours = newHours;
                        
                        // Send modified record
                        Request modifyRequest;
                        modifyRequest.op = Operation::MODIFY_RESPONSE;
                        modifyRequest.recordId = recordId;
                        modifyRequest.employee = modifiedEmp;
                        
                        if (!writeToPipe(pipeHandle, &modifyRequest, sizeof(Request))) {
                            cerr << "Error: Failed to send modified record" << endl;
                            break;
                        }
                        
                        cout << "Record modified successfully!" << endl;
                        
                        // Release the lock
                        Request releaseRequest;
                        releaseRequest.op = Operation::EXIT;
                        writeToPipe(pipeHandle, &releaseRequest, sizeof(Request));
                    } else {
                        cerr << "Error: " << response.message << endl;
                    }
                    break;
                }
                case 3: { // Exit
                    Request request;
                    request.op = Operation::EXIT;
                    writeToPipe(pipeHandle, &request, sizeof(Request));
                    running = false;
                    break;
                }
            }
        }
        
        closePipe(pipeHandle);
        consolePrint("\n=== Client terminated ===\n");
        
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

//
//  common.h
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#ifndef COMMON_H
#define COMMON_H

#include <cstddef>
#include <string>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#endif

// Constants
static constexpr int MAX_NAME_LEN = 10;
static constexpr int MAX_RECORDS = 1000;
static constexpr int BUFFER_SIZE = 256;
static constexpr int MAX_CLIENTS = 10;
static constexpr int PIPE_NAME_MAX = 108;

// Employee structure
struct Employee {
    int num;        // Employee ID
    char name[MAX_NAME_LEN];
    double hours;   // Hours worked
};

// Operation codes for client-server communication
enum class Operation {
    READ = 1,
    MODIFY = 2,
    EXIT = 3,
    READ_RESPONSE = 4,
    MODIFY_REQUEST = 5,
    MODIFY_RESPONSE = 6,
    LOCK_ACQUIRED = 7,
    LOCK_RELEASED = 8,
    ERROR = 9
};

// Request structure from client to server
struct Request {
    Operation op;
    int recordId;
    Employee employee;
    int clientId;
};

// Response structure from server to client
struct Response {
    Operation op;
    int status;      // 0 = success, -1 = error
    Employee employee;
    char message[BUFFER_SIZE];
};

// Named pipe wrapper (cross-platform)
#ifdef _WIN32
    typedef HANDLE PipeHandle;
    #define INVALID_PIPE INVALID_HANDLE_VALUE
#else
    typedef int PipeHandle;
    #define INVALID_PIPE -1
#endif

// Function declarations
void sleepMs(int milliseconds);
void consolePrint(const std::string& message);
bool createNamedPipe(const std::string& pipeName, PipeHandle& handle);
bool waitForClient(PipeHandle& handle);
bool connectToPipe(const std::string& pipeName, PipeHandle& handle);
bool writeToPipe(PipeHandle handle, const void* data, size_t size);
bool readFromPipe(PipeHandle handle, void* data, size_t size);
void closePipe(PipeHandle handle);
std::string getPipeName();

#endif

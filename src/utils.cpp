//
//  utils.cpp
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#include "../include/common.h"
#include <iostream>
#include <mutex>

using std::string;
using std::mutex;
using std::lock_guard;

static mutex consoleMutex;

void sleepMs(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void consolePrint(const string& message) {
    lock_guard<mutex> lock(consoleMutex);
    std::cout << message << std::flush;
}

#ifdef _WIN32
string getPipeName() {
    return "\\\\.\\pipe\\lab5_pipe";
}
#else
string getPipeName() {
    return "/tmp/lab5_pipe";
}
#endif

bool createNamedPipe(const string& pipeName, PipeHandle& handle) {
#ifdef _WIN32
    handle = CreateNamedPipeA(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFFER_SIZE,
        BUFFER_SIZE,
        0,
        NULL
    );
    return handle != INVALID_PIPE;
#else
    unlink(pipeName.c_str());
    handle = socket(AF_UNIX, SOCK_STREAM, 0);
    if (handle == INVALID_PIPE) return false;
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, pipeName.c_str(), sizeof(addr.sun_path) - 1);
    
    if (bind(handle, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(handle);
        return false;
    }
    
    if (listen(handle, MAX_CLIENTS) == -1) {
        close(handle);
        return false;
    }
    
    return true;
#endif
}

bool waitForClient(PipeHandle& handle) {
#ifdef _WIN32
    return ConnectNamedPipe(handle, NULL) != 0;
#else
    struct sockaddr_un clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    handle = accept(handle, (struct sockaddr*)&clientAddr, &clientLen);
    return handle != INVALID_PIPE;
#endif
}

bool connectToPipe(const string& pipeName, PipeHandle& handle) {
#ifdef _WIN32
    handle = CreateFileA(
        pipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    return handle != INVALID_PIPE;
#else
    handle = socket(AF_UNIX, SOCK_STREAM, 0);
    if (handle == INVALID_PIPE) return false;
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, pipeName.c_str(), sizeof(addr.sun_path) - 1);
    
    return connect(handle, (struct sockaddr*)&addr, sizeof(addr)) == 0;
#endif
}

bool writeToPipe(PipeHandle handle, const void* data, size_t size) {
#ifdef _WIN32
    DWORD bytesWritten;
    return WriteFile(handle, data, size, &bytesWritten, NULL) != 0;
#else
    return send(handle, data, size, 0) == (ssize_t)size;
#endif
}

bool readFromPipe(PipeHandle handle, void* data, size_t size) {
#ifdef _WIN32
    DWORD bytesRead;
    return ReadFile(handle, data, size, &bytesRead, NULL) != 0;
#else
    return recv(handle, data, size, 0) == (ssize_t)size;
#endif
}

void closePipe(PipeHandle handle) {
#ifdef _WIN32
    FlushFileBuffers(handle);
    DisconnectNamedPipe(handle);
    CloseHandle(handle);
#else
    shutdown(handle, SHUT_RDWR);
    close(handle);
#endif
}

# Lab5_OS - Named Pipes (Client-Server)

## Description
A cross-platform C++ program demonstrating inter-process communication using **named pipes** with record-level locking.

## Architecture
- **Server**: Manages employee database, handles client requests with proper locking
- **Client**: Connects to server, performs read/modify operations on records

## Features
- Named pipe communication (cross-platform: POSIX sockets / Windows pipes)
- Record-level locking (shared locks for reads, exclusive locks for writes)
- Concurrent client support (up to 10 clients)
- Binary file storage for employee records
- Deadlock prevention
- Comprehensive unit tests (12 tests)

## Building

```bash
chmod +x run_tests.sh
./run_tests.sh
```

## Running

### Terminal 1 (Server)
```bash
cd build
./server.exe
```

### Terminal 2 (Client 1)
```bash
cd build
./client.exe
```

### Terminal 3 (Client 2) - optional
```bash
cd build
./client.exe
```

## Example Session

### Server Input:
```
Enter employee file name: employees.dat
Enter number of employees: 3

Employee #1
Enter name: Ivan Petrov
Enter hours worked: 160

Employee #2
Enter name: Anna Sidorova
Enter hours worked: 155

Employee #3
Enter name: Sergey Ivanov
Enter hours worked: 168

Enter number of clients: 2
```

### Client Menu:
```
--- Client Menu ---
1. Read record
2. Modify record
3. Exit
Enter choice: 1
Enter record ID: 2

=== Employee Record ===
ID: 2
Name: Anna Sidorova
Hours: 155
```

## Locking Mechanism

| Lock Type | Multiple Readers | Writer Allowed |
|-----------|-----------------|----------------|
| Shared (Read) | Yes | No |
| Exclusive (Write) | No | No (only one) |

## Test Results

```
[==========] 12 tests from 2 test suites ran.
[  PASSED  ] 12 tests.

- CreatesFileSuccessfully ✅
- ReadsRecordCorrectly ✅
- WritesRecordCorrectly ✅
- LockUnlockRecordWorks ✅
- SharedLocksAllowMultipleReaders ✅
- ExclusiveLockPreventsSharedLock ✅
- RecordExists ✅
- GetRecordCount ✅
- SleepMsDoesNotCrash ✅
- ConsolePrintWorks ✅
- GetPipeNameReturnsNonEmptyString ✅
- PipeNameHasCorrectFormat ✅
```

## Requirements Met

| Requirement | Implementation |
|-------------|----------------|
| Binary file with employee structure | `EmployeeFile` class |
| Named pipe communication | Cross-platform pipe wrapper |
| Multiple clients | Up to 10 concurrent clients |
| Record locking | Shared/exclusive lock system |
| Read operation | Shared lock, no blocking for reads |
| Modify operation | Exclusive lock, blocks other clients |
| ID-based access | Record ID as key |
| Cross-platform | POSIX + Windows APIs |

## Project Structure

```
lab5_os/
├── include/
│   ├── common.h          # Shared constants and types
│   └── employee.h        # EmployeeFile class
├── src/
│   ├── server.cpp        # Server process
│   ├── client.cpp        # Client process
│   ├── employee.cpp      # Employee file operations
│   └── utils.cpp         # Utility functions
├── tests/
│   ├── test_runner.cpp   # Google Test runner
│   ├── test_employee.cpp # Employee tests (8)
│   └── test_utils.cpp    # Utils tests (4)
├── CMakeLists.txt        # CMake configuration
└── run_tests.sh          # Build and test script
```

## Cross-Platform Support

| Feature | macOS/Linux | Windows |
|---------|-------------|---------|
| Named pipe | Unix domain sockets | Named pipes (\\\\.\\pipe\\) |
| Thread sync | pthread_mutex_t | CRITICAL_SECTION |
| File locking | flock | LockFileEx |

## Author
Elizaveta Kudinova
Group 12

## Repository
[lab 5](https://github.com/luvlizzie/lab5_os)


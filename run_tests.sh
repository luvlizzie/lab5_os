#
//  run_tests.sh
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#!/bin/bash

echo "=== Named Pipe Lab5 - Build and Test ==="
echo "Version 1.0"
echo "========================================"

# Clean previous build
rm -rf build
mkdir build
cd build

# Configure and build
cmake ..
make

# Run tests
echo -e "\n=== Running Tests ===\n"
./test_runner

# Test results
if [ $? -eq 0 ]; then
    echo -e "\n All tests passed!"
else
    echo -e "\n Some tests failed!"
fi

echo -e "\n=== Build completed ==="
echo "To run:"
echo "  Terminal 1: ./server.exe"
echo "  Terminal 2: ./client.exe"
echo ""
echo "Note: Run server first, then clients in separate terminals"

cd ..

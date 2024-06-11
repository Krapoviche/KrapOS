#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;33m'
NC='\033[0m'

# Prepare environment
rm -rf run_tests
mkdir -p run_tests
cp -r ./* run_tests
cd run_tests

# Patched start.c to handle on-the-fly edition
rm kernel/start.c && mv kernel/tests/start.c kernel/start.c

# Patched screen.c to `printf` directly on 0xE9 using `outb`
rm kernel/screen.c && mv kernel/tests/screen.c kernel/screen.c

make clean

# Store original value of start.c
start_c=$(cat kernel/start.c)

# For each test case
for test_file in $(ls -v kernel/tests | grep test | grep -v expected ); do
    echo -n "RUNNING TEST $test_file : "

    # Edit start on-the-fly for the test-case
    sed -i "s/TEST_CASE/$test_file/g" kernel/start.c

    # Build kernel.bin
    if ! make -s > /dev/null ; then
        echo -e "${RED}FAILURE${NC}"
        echo "$start_c" > kernel/start.c
        continue
    fi

    # Run kernel with newly built kernel.bin (with test cases)
    make -s run-kernel-test > /dev/null

    # Loop vars
    i=0
    pattern=$(< kernel/tests/$test_file.expected)

    # Check wether time is up or the run output matches the exepected regex
    while ! grep -Eq "$pattern" qemu-output.txt && ! [ $i -eq 10 ]; do
        sleep 1
        i=$((i+1))
    done

    # Display failure or success
    if grep -Eq "$pattern" qemu-output.txt ; then
        echo -e "${GREEN}SUCCESS${NC}"
    else
        echo -e "${RED}FAILURE${NC}"
        echo "Expected : $(cat kernel/tests/$test_file.expected)"
        echo "Received : $(cat qemu-output.txt)"
    fi

    # End kernel run
    killall qemu-system-i386 > /dev/null 2> /dev/null

    # Store output
    mv qemu-output.txt kernel/tests/$test_file.out

    # Reset the status of patched start.c
    echo "$start_c" > kernel/start.c
done

# Store test results
cp -r kernel/tests ../kernel-test-report

# Cleanup
cd ../
rm -rf run_tests

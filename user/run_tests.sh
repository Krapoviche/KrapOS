#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;33m'
NC='\033[0m'

# Prepare environment
rm -rf run_tests
make clean
mkdir -p run_tests
cp -r ./* run_tests
cp -r ./tests ./run_tests/user
cd run_tests

# Patched start.c to handle on-the-fly edition
rm user/start.c && mv user/tests/start.c user/start.c
rm kernel/start.c && mv user/tests/kernel-start.c kernel/start.c

# Patched screen.c to `printf` directly on 0xE9 using `outb`
rm kernel/screen.c && mv user/tests/screen.c kernel/screen.c

# Store original value of start.c
start_c=$(cat user/start.c)

# For each test case
for test_file in $(ls -v user/tests | grep test | grep -v expected ); do
    echo -n "RUNNING TEST $test_file : "

    # Edit start on-the-fly for the test-case
    sed -i "s/TEST_CASE/$test_file/g" user/start.c

    # Build kernel.bin
    if ! make -s > /dev/null ; then
        echo -e "${RED}FAILURE${NC}"
        echo "$start_c" > user/start.c
        continue
    fi


    # Run kernel with newly built kernel.bin (with test cases)
    make -s run-kernel-test > /dev/null

    # Loop vars
    i=0
    pattern=$(< user/tests/$test_file.expected)

    if [[ $test_file == *"test19"* ]] ; then
        echo -e "${RED}This test can only be run manually since it requires user keyboard input${NC}"
    else

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
            echo "Expected : $(cat user/tests/$test_file.expected)"
            echo "Received : $(cat qemu-output.txt)"
        fi
    fi

    # End kernel run
    killall qemu-system-i386 > /dev/null 2> /dev/null

    # Store output
    mv qemu-output.txt user/tests/$test_file.out

    # Reset the status of patched start.c
    echo "$start_c" > user/start.c
done

# Store test results
cp -r user/tests ../user-test-report

# Cleanup
cd ../
rm -rf run_tests

#!/bin/bash

# ==============================
# Configuration
# ==============================
PROJ_DIR="/home/aris/project/rvemu"
EMU="${PROJ_DIR}/rvemu"
TEST_DIR="${PROJ_DIR}/riscv-tests/isa"
REPORT="${PROJ_DIR}/tmp/test_report.txt"
FAIL_LIST="${PROJ_DIR}/tmp/test_failures.txt"

# Clear previous reports
> "$REPORT"
> "$FAIL_LIST"

PASS=0
FAIL=0
TOTAL=0

# ==============================
# Test Suites
# ==============================
SUITES="rv64ui rv64um rv64uf rv64ud rv64uc"

echo "================================================"
echo "          RISC-V TVM Test Runner"
echo "================================================"

for suite in $SUITES; do
    echo -e "\n>>> Test Suite: $suite"

    for bin in "$TEST_DIR"/"$suite"-p-*.bin; do
        if [ ! -f "$bin" ]; then
            continue
        fi

        TOTAL=$((TOTAL + 1))
        name=$(basename "$bin")

        echo -n "Testing: $name ... "

        # Run emulator
        output=$($EMU "$bin" 2>&1)
        exit_code=$?

        if [ "$exit_code" -eq 0 ]; then
            echo -e "\033[32mPASS\033[0m"
            echo "[PASS] $name" >> "$REPORT"
            PASS=$((PASS + 1))
        else
            echo -e "\033[31mFAIL\033[0m (code: $exit_code)"
            echo "[FAIL] $name (exit: $exit_code)" >> "$REPORT"
            echo "$name" >> "$FAIL_LIST"
            FAIL=$((FAIL + 1))
        fi
    done
done

# ==============================
# Final Report
# ==============================
echo -e "\n================================================"
echo "Testing completed!"
echo "Total tests: $TOTAL"
echo "Passed:      $PASS"
echo "Failed:      $FAIL"
echo "================================================"

if [ "$FAIL" -gt 0 ]; then
    echo -e "\nFailed tests are logged in: $FAIL_LIST"
else
    echo -e "\n✅ All tests passed!"
fi

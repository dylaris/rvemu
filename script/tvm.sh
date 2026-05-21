#!/bin/bash

# ==============================
# Configuration
# ==============================
PROJ_DIR="/home/aris/project/rvsim"
EMU="${PROJ_DIR}/rvsim"
TEST_DIR="${PROJ_DIR}/riscv-tests/build/share/riscv-tests/isa/"
REPORT_DIR="${PROJ_DIR}/report/tvm"
REPORT="${REPORT_DIR}/tvm_report.txt"
FAIL_LIST="${REPORT_DIR}/tvm_failures.txt"

mkdir -p $REPORT_DIR
> "$REPORT"
> "$FAIL_LIST"

PASS=0
FAIL=0
TOTAL=0

# ==============================
# Test Suites
# ==============================
SUITES="rv64ui rv64um rv64uc"

echo "================================================"
echo "          RISC-V TVM Test Runner"
echo "================================================"

for suite in $SUITES; do
    echo -e "\n>>> Test Suite: $suite"

    pass_bins=()
    fail_bins=()

    for bin in "$TEST_DIR"/"$suite"-p-*.bin; do
        [ -f "$bin" ] || continue
        TOTAL=$((TOTAL + 1))
        name=$(basename "$bin")

        output=$($EMU --bin "$bin" 2>&1)
        exit_code=$?

        if [ "$exit_code" -eq 0 ]; then
            pass_bins+=("$name")
            echo "[PASS] $name" >> "$REPORT"
            PASS=$((PASS + 1))
        else
            fail_bins+=("$name")
            echo "[FAIL] $name (exit: $exit_code)" >> "$REPORT"
            echo "$name" >> "$FAIL_LIST"
            FAIL=$((FAIL + 1))
        fi
    done

    # Print compressed summary line
    if [ ${#pass_bins[@]} -gt 0 ]; then
        printf "Testing: { %s }: \033[32mAll PASS\033[0m\n" "$(IFS=', '; echo "${pass_bins[*]}")"
    fi
    if [ ${#fail_bins[@]} -gt 0 ]; then
        printf "\033[31mFailed: { %s }\033[0m\n" "$(IFS=', '; echo "${fail_bins[*]}")"
    fi
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

[ "$FAIL" -gt 0 ] && echo -e "\nFailed tests are logged in: $FAIL_LIST" || echo -e "\n✅ All tests passed!"

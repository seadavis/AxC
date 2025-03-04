#!/bin/bash

# Define paths
RUN_SCRIPT="./run.sh"
TEST_STRING="Hello, Test!"

# Run the first script with the test string and capture output
OUTPUT=$($RUN_SCRIPT -- "$TEST_STRING")

# Define expected output (modify this based on your actual program behavior)
EXPECTED_OUTPUT="Running build/bin/AxC Hello, Test!
Hello, Test!"  # Adjust if needed

# Compare outputs
if [[ "$OUTPUT" == "$EXPECTED_OUTPUT" ]]; then
    echo "✅ Test Passed: Output matches expected result."
    exit 0
else
    echo "❌ Test Failed: Output does not match expected result."
    echo "Expected:"
    echo "$EXPECTED_OUTPUT"
    echo "Got:"
    echo "$OUTPUT"
    exit 1
fi
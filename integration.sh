#!/bin/bash

# Function to handle SIGINT (Ctrl+C)
cleanup() {
   # Kill all child processes spawned by this script
   kill $(jobs -p) >/dev/null 2>&1
   exit 1
}

# Trap SIGINT and call the cleanup function
trap cleanup SIGINT

# Build the project without running any tests
make -j -f tdd.mk INTEGRATION=Y group- >/dev/null 2>&1 || :

# Fetch all group names from the rockhopper binary and store them as a space-separated string
GROUP_NAMES=$(build/test/rockhopper -lg)

# Convert the space-separated list into an array
IFS=' ' read -ra ALL_GROUPS <<<"$GROUP_NAMES"

# Filter the array for group names containing 'Integration'
INTEGRATION_TESTS=()
for group_name in "${ALL_GROUPS[@]}"; do
   if [[ "$group_name" == *"Integration"* ]]; then
      INTEGRATION_TESTS+=("$group_name")
   fi
done

# Function to run a single test group
run_test() {
   local test_group="$1"
   local output=$(build/test/rockhopper $test_group)
   echo '----------------------------------------'
   echo "Ran $test_group"
   echo "$output"
   echo ""
}

# Run all Integration tests in parallel
for test_group in "${INTEGRATION_TESTS[@]}"; do
   run_test "-g $test_group" &
done

# Wait for all background processes to complete
wait

echo "Integration tests complete"

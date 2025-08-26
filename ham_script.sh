#!/bin/bash
# A script to generate and construct a qubit Hamiltonian.

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Configuration ---
# Define paths to key components for easier maintenance.
PROJECT_DIR="$(pwd)"
PYTHON_SCRIPT="quantum_chemistry/hamiltonian_generator.py"
CPP_FILE="quantum_chemistry/hamiltonian_constructor.cpp"
OUTPUT_FILE="hamiltonian.txt"
VIRTUAL_ENV="virtual_env"

# Intel Quantum SDK specific paths
# These are coded based on the docker container setup.
IQS_BASE_PATH="/opt/intel/quantum-sdk/docker-intel_quantum_sdk_1.1.1.2024-11-15T22_03_32+00_00"
IQS_LIB_PATH="${IQS_BASE_PATH}/Hybrid-Quantum-Classical-Library/build/lib"
IQS_INCLUDE_PATH="${IQS_BASE_PATH}/Hybrid-Quantum-Classical-Library/build/include"

echo "Starting Hamiltonian generation and construction..."
echo "---------------------------------------------------"

# --- Step 1: Generate Hamiltonian using OpenFermion (Python) ---
echo "1. Activating Python virtual environment and generating Hamiltonian..."
# Check if the virtual environment exists before trying to activate.
if [ ! -d "${VIRTUAL_ENV}" ]; then
    echo "Error: Python virtual environment not found at ${VIRTUAL_ENV}. Please create and install dependencies."
    exit 1
fi
source "${VIRTUAL_ENV}/bin/activate"

# Check if the Python script exists before executing.
if [ ! -f "${PYTHON_SCRIPT}" ]; then
    echo "Error: Python script not found at ${PYTHON_SCRIPT}."
    exit 1
fi

python3 "${PYTHON_SCRIPT}"

# Deactivate the virtual environment.
deactivate
echo "   Successfully generated '${OUTPUT_FILE}'."

# --- Step 2: Compile and Execute C++ code (Intel QSDK) ---
echo "2. Compiling C++ module using Intel Quantum Compiler..."
# Set the library path for the compiler.
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$IQS_LIB_PATH

# Check if the C++ file exists before compiling.
if [ ! -f "${CPP_FILE}" ]; then
    echo "Error: C++ source file not found at ${CPP_FILE}."
    exit 1
fi

# Use the full path for the compiler and specify output executable name.
./intel-quantum-compiler -I"${IQS_INCLUDE_PATH}" -L"${IQS_LIB_PATH}" -larmadillo -lhqcl "${CPP_FILE}"

echo "   Compilation successful."

echo "3. Executing C++ program to construct and print the Hamiltonian..."
# Check if the executable exists before running.
if [ ! -f "./hamiltonian_constructor" ]; then
    echo "Error: Executable not found. Compilation failed."
    exit 1
fi

# Execute the compiled program.
./hamiltonian_constructor

echo "---------------------------------------------------"
echo "Workflow completed successfully."

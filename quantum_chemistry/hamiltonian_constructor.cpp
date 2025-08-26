#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/quintrinsics.h>
#include <qexpr_utils.h>
#include <quantum_full_state_simulator_backend.h>

#include <quantum.hpp>
#include "SymbolicOperator.hpp"
#include "SymbolicOperatorUtils.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <complex>
#include <regex>

namespace hqcl = hybrid::quantum::core;
using pstring = std::vector<std::pair<int, char>>;

// A function to parse a line from the OpenFermion output file.
// It separates the coefficient and the Pauli string.
bool parseLine(const std::string& line, double& coefficient, pstring& pauli_string) {
    // Regex to match the complex number part.
    std::regex coeff_regex("\\((-?\\d+\\.\\d+e?[-+]?\\d*)\\+0j\\)");
    std::smatch coeff_match;

    if (!std::regex_search(line, coeff_match, coeff_regex)) {
        std::cerr << "Failed to parse coefficient for line: " << line << std::endl;
        return false;
    }

    // Extract the numerical coefficient (the first captured group).
    coefficient = std::stod(coeff_match[1].str());
    //std::cout << "coefficient = " << coeff_match[1].str() << std::endl;

    // Regex to match the Pauli string, e.g., "[X0 Y1 Z2]".
    std::regex pauli_regex("\\[([XYZ]\\d+\\s?)*\\]");
    std::smatch pauli_match;

    if (std::regex_search(line, pauli_match, pauli_regex)) {
        std::string pauli_str = pauli_match[0].str();
        pauli_string.clear();

        // If it's not the identity term (just []), parse the operators.
        if (pauli_str.length() > 2) {
            std::regex op_regex("([XYZ])(\\d+)");
            std::sregex_iterator next(pauli_str.begin(), pauli_str.end(), op_regex);
            std::sregex_iterator end;

            while (next != end) {
                std::smatch op_match = *next;
                char pauli_char = op_match[1].str()[0];
                int qubit_index = std::stoi(op_match[2].str());
                pauli_string.push_back({qubit_index, pauli_char});
                ++next;
            }
        }
    } else {
        std::cerr << "Failed to parse Pauli string for line: " << line << std::endl;
        return false;
    }

    return true;
}

// Function to read the Hamiltonian from a file and build the SymbolicOperator.
hqcl::SymbolicOperator loadHamiltonianFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return hqcl::SymbolicOperator();
    }

    hqcl::SymbolicOperator hamiltonian_symbop;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.find("#") == 0) {
            continue; // Skip empty lines and comments
        }
        
        double coefficient;
        pstring term;
        if (parseLine(line, coefficient, term)) {
            std::set<std::pair<int,char>> term_set(term.begin(), term.end());
            hamiltonian_symbop.addTerm(term_set, coefficient);
        }
    }
    file.close();
    return hamiltonian_symbop;
}

int main() {
    const std::string filename = "hamiltonian.txt";
    // Now, load the Hamiltonian from the file.
   hqcl::SymbolicOperator H_symbop = loadHamiltonianFromFile(filename);

    // Get the character string representation to verify the result.
    std::string charstring = H_symbop.getCharString();
    std::cout << "\nConstructed Hamiltonian:\n" << charstring << "\n";

    return 0;
}

from openfermion.chem import MolecularData
from openfermion.transforms import get_fermion_operator, jordan_wigner
from openfermion.utils import save_operator
from openfermionpyscf import run_pyscf
import os

# Define the molecule and run a PySCF calculation to get integrals
geometry = [('H', (0., 0., 0.)), ('H', (0., 0., 0.7414))]
basis = 'sto-3g'
multiplicity = 1
charge = 0
description = 'H2_jw'

molecule = MolecularData(geometry, basis, multiplicity, charge, description)
molecule = run_pyscf(molecule, run_scf=1)

# Get the molecular Hamiltonian
molecular_hamiltonian = molecule.get_molecular_hamiltonian()
#print("-----Molecular Hamiltonian-----")
#print(molecular_hamiltonian)

# Get the fermionic Hamiltonian
fermionic_hamiltonian = get_fermion_operator(molecular_hamiltonian)
#print("-----Fermionic Hamiltonian-----")
#print(fermionic_hamiltonian)

# Apply the Jordan-Wigner transformation
qubit_hamiltonian = jordan_wigner(fermionic_hamiltonian)
print("-----Qubit Hamiltonian-----")
print(qubit_hamiltonian)

# Save the qubit Hamiltonian to a plain text file
file_name = "hamiltonian.txt"
with open(file_name, "w") as f:
    f.write(str(qubit_hamiltonian))
#save_operator(qubit_hamiltonian, file_name=file_name, plain_text=True, allow_overwrite=True)

print(f'Qubit Hamiltonian saved to {os.getcwd()}/{file_name}')

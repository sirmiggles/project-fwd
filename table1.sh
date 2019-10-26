#  Miguel Aries Sambat Tabadero
#  Student Number = 22240204

#PBS -l nodes=16:ppn=4
#PBS -m abe
#PBS -M 22240204@student.uwa.edu.au
#PBS -e error_table1.txt
#PBS -o output_table1.txt
source /etc/bash.bashrc
syncCluster
cd ./project

# table I - 2048 vs different processors
mpirun -n 1 fwd sample/2048.in
mpirun -n 2 fwd sample/2048.in
mpirun -n 4 fwd sample/2048.in
mpirun -n 8 fwd sample/2048.in
mpirun -n 16 fwd sample/2048.in



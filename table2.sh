#  Miguel Aries Sambat Tabadero
#  Student Number = 22240204

#PBS -l nodes=8:ppn=4
#PBS -m abe
#PBS -M 22240204@student.uwa.edu.au
#PBS -e error_table2.txt
#PBS -o output_table2.txt
source /etc/bash.bashrc
syncCluster
cd ./project

# table 2 - 4 processors vs 256, 512, 1024, 2048, 4096
mpirun -n 4 fwd sample/256.in
mpirun -n 4 fwd sample/512.in
mpirun -n 4 fwd sample/1024.in
mpirun -n 4 fwd sample/2048.in
mpirun -n 4 fwd sample/4096.in



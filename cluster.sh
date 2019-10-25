#PBS -l nodes=8:ppn=4
#PBS -m abe
#PBS -M 22240204@student.uwa.edu.au
#PBS -e error.txt
#PBS -o out.txt
source /etc/bash.bashrc
syncCluster
cd ./project
make
mpirun fwd sample/4.in
echo "done with 4"
mpirun fwd sample/16.in
echo "done with 16"
mpirun fwd sample/32.in
echo "done with 32"
mpirun fwd sample/512.in
echo "done with 512"
mpirun fwd sample/512.in
echo "done with 512"
mpirun fwd sample/1024.in
echo "done with 1024"

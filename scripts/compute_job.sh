#!/bin/bash
#SBATCH --job-name="test-GNU"
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jfm343@cornell.edu
#SBATCH --partition=compute
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=64
#SBATCH --mem=240G
#SBATCH -t 04:00:00
#SBATCH --account=crl171
#SBATCH --output="job.%j.%N.out"
#SBATCH --error="job.%j.%N.err"
#SBATCH --ntasks-per-core=1
#SBATCH --cpus-per-task=2
#SBATCH --export=ALL


#This job runs with 1 nodes, 128 cores per node for a total of 64 tasks * 2 OpenMP threads.

module purge
module load cpu

#Load module file(s) into the shell environment
module load gcc openblas openmpi hdf5/1.10.1
module load slurm

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
echo $OMP_NUM_THREADS
export OPENBLAS_NUM_THREADS=$OMP_NUM_THREADS
export OMP_PROC_BIND='close'

#compute node local drive:  /scratch/$USER/job_$SLURM_JOB_ID


#running
time mpirun -mca shmem mmap -np $SLURM_NTASKS --bind-to core --map-by ppr:$SLURM_NTASKS_PER_NODE:node:pe=$SLURM_CPUS_PER_TASK --report-bindings  SSE
cat info

##moving data out
date_fin="`date "+%Y-%m-%d-%H-%M-%S"`"
dirfin="${PRDIR}/${SLURM_JOB_NAME}_${date_fin}"
mkdir "${dirfin}"
mv * "${dirfin}"
#!/bin/bash
#SBATCH --job-name="larger"
#SBATCH --output="job.%j.%N.out"
#SBATCH --error="job.%j.%N.err"
#SBATCH --partition=compute
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=16
##SBATCH --ntasks-per-core=1
#SBATCH --cpus-per-task=8
#SBATCH --mem=200G
#SBATCH --account=crl171
#SBATCH --export=ALL
#SBATCH -t 48:00:00
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jfm343@cornell.edu

#This job runs with 4 nodes, 128 cores per node for a total of 128 tasks.

module purge
module load cpu
#iLoad module file(s) into the shell environment
module load slurm
module load intel intel-mpi intel-mkl

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
echo $OMP_NUM_THREADS
export KMP_AFFINITY=granularity=core,compact,verbose
export I_MPI_PMI_LIBRARY=/cm/shared/apps/slurm/current/lib64/libpmi.so

#compute node local drive:  /scratch/$USER/job_$SLURM_JOB_ID
srun SSE

cat info

##moving data out
date_fin=$(date "+%Y-%m-%d-%H-%M-%S")
dirfin="${PRDIR}/${SLURM_JOB_NAME}_${date_fin}"
mkdir "${dirfin}"
mv * "${dirfin}"

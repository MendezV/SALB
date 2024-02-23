#!/bin/bash
###########################
#
#Scritp that reads parameter file and
#runs a QMC simulation for each parameter
#in the file
#
#
###########################

#Reading parameter file
#readarray -t parameter_array < Hops.dat
parameter_array_0=$(awk -F= '{print $1}' parrarr_0.dat) #beta
parameter_array_1=$(awk -F= '{print $1}' parrarr_1.dat) #size


jobname="Heisenberg_test"  #JOBNAME importan to declare -has to be descriptive
pow=$PWD #saving the current working directory


#General info about the job
date_in=$(date "+%Y-%m-%d-%H-%M-%S")
echo $date_in

#loop over the parameters
for parameter_0 in ${parameter_array_0[@]}; do

	for parameter_1 in ${parameter_array_1[@]}; do
	
        echo "${pow} is the current working directory"
        #create one temporary directory per parameter
        jname="${jobname}_${parameter_0}_${parameter_1}_${parameter_2}"

        #modifying "parameter" file for the specific run
        #and moving everything to the temp directory
        
        #beta
        sed "s:beta=10.0:beta=${parameter_0}:g"  parameters2 > "parameters1_${parameter_0}"

        #size
        sed "s:Lx=8:Lx=${parameter_1}!:g"  "parameters1_${parameter_0}" > "parameters2_${parameter_0}"
        sed "s:Ly=8:Ly=${parameter_1}!:g"  "parameters2_${parameter_0}" > "parameters3_${parameter_0}"
        sed "s:Lz=8:Lz=${parameter_1}!:g"  "parameters3_${parameter_0}" > "parameters4_${parameter_0}"


        #creating temp directory in scratch to run
        dir="${TMPDIR}/${jname}_${date_in}"
        mkdir -v "${dir}"

        #moving seeds, parameters and TBG_landau.out to the new dir
        echo "started copy of job files...."
        cp "parameters4_${parameter_0}"  "${dir}/parameters.txt"

        #removing extra parameter files
        rm "parameters1_${parameter_0}"
        rm "parameters2_${parameter_0}"
        rm "parameters3_${parameter_0}"
        rm "parameters4_${parameter_0}"
        
        #copy the other files executable, seeds and scripts
        cp SSE "${dir}" #needs to be in the same directory
        cp seeds "${dir}"
        cp compute_job_intel.sh "${dir}"
        cp compute_run_param_intel.sh "${dir}"
        echo "ended copy of job files...."

        #entering the temp directory, running and coming back
        echo "submitting job...."
        cd "${dir}"
        sbatch -J "${jname}" compute_job_intel.sh
        sleep 1
        echo "moving back to ${pow} ...."
        cd "${pow}"

    
	done

done

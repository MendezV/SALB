import os
import shutil



SALB_DIR = os.environ.get("SALB_DIR","home/user")

# Constants
TEMP_DIR = "/home/jfm343/Documents/wigner-mott/tmpdir"  # Replace with your temp directory path
TEMPLATE_FILE = os.path.join(SALB_DIR,"init/parameters.txt")  # Replace with your template file path
EXEC_FILE = os.path.join(SALB_DIR,"build/SSE")  # Replace with your template file path


# Function to modify the template file
def modify_template(file_path, sim):
    
    # looping over the dictionary that contains the parameters of the simulation
    for parameter_name in sim:
        
        parameter_value = sim[parameter_name] #parameter value 
        PARAMETER_NAME = parameter_name +"=" #formatting 
        
        #reading the file
        with open(file_path, 'r') as file:
            lines = file.readlines()

        #writting a new file but modifying the line that contains the parameter
        with open(file_path, 'w') as file:
            for line in lines:
                if line.startswith(PARAMETER_NAME):
                    file.write(f"{PARAMETER_NAME}{parameter_value}\n")
                else:
                    file.write(line)

def gen_sim_name(jobname, sim):
    
    name = jobname
    for parameter_name in sim:
        name= f"{name}_{parameter_name}_{sim[parameter_name]}"
        
    return name


def prep_sim_dir(sim):
    
    # Create subdirectory for each parameter
    simname  = gen_sim_name(jobname, sim)
    sub_dir = os.path.join(TEMP_DIR, simname)
    os.makedirs(sub_dir, exist_ok=True)

    # Copy the template file to the subdirectory
    sub_file_path = os.path.join(sub_dir, "parameters.txt")
    shutil.copy(TEMPLATE_FILE, sub_file_path)

    # Modify the template file in the subdirectory
    modify_template(sub_file_path, sim)
    
    # Copy the executable file to the subdirectory
    sub_file_path = os.path.join(sub_dir, "SSE")
    shutil.copy(EXEC_FILE, sub_file_path)

    return None
    

jobname = "heisenberg"
sims = []
import numpy as np
betavals = 1/np.linspace(0.25,3.0,15)
Lvals = [8,12,16]

for beta in betavals:
    for L in Lvals:
        
        sim = {
            "beta":beta,
            "Lx":L,
            "Ly":L,
            "Lz":L
        }
        
        sims.append(sim)
        


# Main script
for sim in sims:
   
   prep_sim_dir(sim) 
   
   
print("Script executed. Directories created and files modified according to the parameters.")

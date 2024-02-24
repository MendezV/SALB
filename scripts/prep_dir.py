import os
import shutil



SALB_DIR = os.environ.get("SALB_DIR","home/user")

# Constants
TEMP_DIR = "/home/felipe/Documents/Projects/wigner-mott/tmpdir"  # Replace with your temp directory path
TEMPLATE_FILE = os.path.join(SALB_DIR,"init/parameters.txt")  # Replace with your template file path
EXEC_FILE = os.path.join(SALB_DIR,"build/SSE")  # Replace with your template file path


# Function to modify the template file
def modify_template(file_path, parameter_name, parameter_value):
    PARAMETER_NAME = parameter_name +"="
    with open(file_path, 'r') as file:
        lines = file.readlines()

    with open(file_path, 'w') as file:
        for line in lines:
            if line.startswith(PARAMETER_NAME):
                file.write(f"{PARAMETER_NAME}{parameter_value}\n")
            else:
                file.write(line)

PARAMETERS = [1.0, 2.0, 3.0]  # List of parameter values
param1 = "beta"

# Main script
for param in PARAMETERS:
    # Create subdirectory for each parameter
    sub_dir = os.path.join(TEMP_DIR, f"heisenberg_{param1}_{param}")
    os.makedirs(sub_dir, exist_ok=True)

    # Copy the template file to the subdirectory
    sub_file_path = os.path.join(sub_dir, "parameters.txt")
    shutil.copy(TEMPLATE_FILE, sub_file_path)

    # Modify the template file in the subdirectory
    modify_template(sub_file_path, param1,  param)
    
    # Copy the executable file to the subdirectory
    sub_file_path = os.path.join(sub_dir, "SSE")
    shutil.copy(EXEC_FILE, sub_file_path)

print("Script executed. Directories created and files modified according to the parameters.")

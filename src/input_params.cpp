#include "input_params.h"

/*
defining the default values of the parameters of the simulation
*/
namespace SimConfig {

    std::string lat_type="cubic";
    int Lx=1;
    int Ly=1;
    int Lz=1;
    int n_therm_sweeps=10;
    int n_sweeps=10;
    int n_bins=10;
    int max_order=1;
    double beta=10.0;    
}

/*
namespace for model variable
*/

namespace ModelConfig {

    std::string model="Heisenberg";

}

/*
namespaces for specific models 
*/
namespace HeisenbergConfig {

    double J=1.0;    
}

//loading the variables for the SimConfig namespace

void loadSimConfig(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        // Print a warning message to the console
        std::cerr << "Error: Unable to open file, taking default params '" << filename << "'" << std::endl;
        exit(EXIT_SUCCESS);
        
    } else {

        // File opened successfully
        

        std::string line;

        while (std::getline(file, line)) {
            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '=')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    if (key == "lat_type") SimConfig::lat_type = value;
                    else if (key == "Lx") SimConfig::Lx = std::stoi(value);
                    else if (key == "Ly") SimConfig::Ly = std::stoi(value);
                    else if (key == "Lz") SimConfig::Lz = std::stoi(value);
                    else if (key == "n_therm_sweeps") SimConfig::n_therm_sweeps = std::stoi(value);
                    else if (key == "n_sweeps") SimConfig::n_sweeps = std::stoi(value);
                    else if (key == "n_bins") SimConfig::n_bins = std::stoi(value);
                    else if (key == "max_order") SimConfig::max_order = std::stoi(value);
                    else if (key == "beta") SimConfig::beta = std::stod(value);
                    
                }
            }
        }
        file.close();
    }
    
}

//loading the variables for the Model-specific namespaces

void loadModelConfig(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        // Print a warning message to the console
        std::cerr << "Error: Unable to open file, taking default params '" << filename << "'" << std::endl;
        exit(EXIT_SUCCESS);
        
    } else {

        std::string line;

        while (std::getline(file, line)) {
            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '=')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    if (key == "model") ModelConfig::model = value;

                    //heisenberg model parameters
                    if ( ModelConfig::model == "Heisenberg" ){
                        if (key == "J") HeisenbergConfig::J = std::stod(value);
                    }
                    
                }
            }
        }
    }
}

int data_file_exists() {
    FILE* file;

    // Try to open the file for reading
    file = fopen(DATA_FILE, "r");

    if (file) {
        // File exists
        fclose(file);
        return 1;
    }

    // File does not exist
    return 0;
}

/*
 * Function: create_hdf5_file_if_not_exists
 * ----------------------------------------
 * Creates an HDF5 file if it does not exist.
 *
 * filename: The name of the HDF5 file to be created.
 *
 * returns: 0 on success, -1 on failure.
 */
int create_hdf5_file_if_not_exists() {
    hid_t file_id; // File identifier

    if (data_file_exists()) {
        // File already exists
        return -1;
    }
    else{
        // Attempt to create the file with H5F_ACC_EXCL flag.
        // This ensures that the file is created only if it does not already exist.
        file_id = H5Fcreate(DATA_FILE, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
        // Close the file.
        H5Fclose(file_id);
        return 0;
    }
    

    
}


int therm_file_exists() {
    FILE* file;

    // Try to open the file for reading
    file = fopen(THERMALIZATION_FILE, "r");

    if (file) {
        // File exists
        fclose(file);
        return 1;
    }

    // File does not exist
    return 0;
}

int create_thermalization_log(){
    
    if (therm_file_exists()) {
        // File already exists
        return -1;
    }
    else{
        // Pointer to the file
        FILE *filePtr;

        // Open the file for writing. If the file does not exist, it will be created.
        // If the file exists, its contents will be overwritten.
        filePtr = fopen(THERMALIZATION_FILE, "w");

        // Check if the file was opened successfully
        if (filePtr == NULL) {
            printf("Error opening file!\n");
            return 1;
        }

        // Write text to the file
        fprintf(filePtr, "sweep \t max_order\n");

        // Close the file
        fclose(filePtr);
        return 0;
    }
    


}

void print_thermalization_adjust(const int& sweep, const int& new_max_order){

    if (therm_file_exists()) {
        // Pointer to the file
        FILE *filePtr;

        // Open the file for appending.
        filePtr = fopen(THERMALIZATION_FILE, "a");

        // Check if the file was opened successfully
        if (filePtr == NULL) {
            printf("Error opening thermalization file!\n");
        }

        // Append text to the file
        fprintf(filePtr, "%d %d \n", sweep, new_max_order);

        // Close the file
        fclose(filePtr);
    }
    else{
        create_thermalization_log();

        // Pointer to the file
        FILE *filePtr;

        // Open the file for appending.
        filePtr = fopen(THERMALIZATION_FILE, "a");

        // Check if the file was opened successfully
        if (filePtr == NULL) {
            printf("Error opening thermalization file!\n");
        }

        // Append text to the file
        fprintf(filePtr, "%d %d \n", sweep, new_max_order);

        // Close the file
        fclose(filePtr);
    }
}

void printarr(int* arr, const int& size){

    for(int i=0;i<size;i++){
        std::cout<<i<<" "<<arr[i]<<std::endl;
    }
}

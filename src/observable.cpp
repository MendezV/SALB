#include "observable.h"

/**
 * @brief Construct a new Observable:: Observable object this constructor works for scalar observables
 * 
 * @param name_in name of the observable that we are calculating
 * @param type_in type of the observable, for this constructor it should be scalar
 */

Observable::Observable()
{

    name = "none";
    type = "scalar";
    n_bins = SimConfig::n_bins;
    obser = new double[n_bins];
    init_obser_scal_h5();

}

Observable::Observable(const char* name_in, std::string type_in)
:name(name_in),type(type_in),n_bins(SimConfig::n_bins)
{
    obser = new double[n_bins];
    init_obser_scal_h5();

}


Observable::~Observable(){

    delete[] obser;
}

void Observable::record_data_h5(const int& bin){

    if(type=="scalar"){
        record_data_scal_h5(bin);
    }

}


void Observable::init_obser_scal_h5(){
    
    int rank = 1;
    hid_t file_id, dataset_id, dataspace_id, prop_id;  // Identifiers
    herr_t status;
    hsize_t dims[rank] = {0};  // One-dimensional array
    hsize_t max_dims[rank] = {H5S_UNLIMITED}; // allows the dataset to be resized to any size
    hsize_t chunk_dims[rank] = {1}; // size of the chunks


    // Open the HDF5 file
    file_id = H5Fopen(DATA_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    if (file_id < 0) {
        fprintf(stderr, "Could not open file %s\n", DATA_FILE);

    }

    // Check if the dataset exists
    if (H5Lexists(file_id, name, H5P_DEFAULT) <= 0) {

        // Create the data space for the dataset
        dataspace_id = H5Screate_simple(rank, dims, max_dims);

        // Create a property list for the dataset and set it to use chunked storage
        prop_id = H5Pcreate(H5P_DATASET_CREATE);
        H5Pset_chunk(prop_id, rank, chunk_dims);
        // Create the dataset
        dataset_id = H5Dcreate(file_id, name, H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, prop_id, H5P_DEFAULT);

        if (dataset_id < 0) {
            fprintf(stderr, "Could not create dataset %s\n", name);
            H5Sclose(dataspace_id);
            H5Fclose(file_id);

        }


        H5Pclose(prop_id);
        // Close the dataset
        H5Dclose(dataset_id);

        // Close the dataspace
        H5Sclose(dataspace_id);
    } 

    // Close the file
    H5Fclose(file_id);

    
}


void Observable::record_data_scal_h5(const int& bin){

    int rank=1;
    hid_t file_id, dataset_id, dataspace_id, memspace_id;
    herr_t status;
    hsize_t dims[rank], max_dims[rank], new_dims[rank];
    hsize_t offset[rank], count[rank];

    // Open the HDF5 file
    file_id = H5Fopen(DATA_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    if (file_id < 0) {
        printf("Error opening file\n");
        return;
    }

    // Open the dataset
    dataset_id = H5Dopen(file_id, name, H5P_DEFAULT);
    if (dataset_id < 0) {
        printf("Error opening dataset\n");
        H5Fclose(file_id);
        return;
    }

    // Get the current size of the dataset
    dataspace_id = H5Dget_space(dataset_id);
    H5Sget_simple_extent_dims(dataspace_id, dims, max_dims);

    // Increment the dataset size by one
    new_dims[0] = dims[0] + 1;
    status = H5Dset_extent(dataset_id, new_dims);
    if (status < 0) {
        printf("Error extending dataset\n");
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        H5Fclose(file_id);
        return;
    }

    // Select a hyperslab in extended portion of dataset
    H5Sclose(dataspace_id);
    dataspace_id = H5Dget_space(dataset_id);
    offset[0] = dims[0];
    count[0] = 1;
    H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);

    // Define memory space
    memspace_id = H5Screate_simple(rank, count, NULL);

    double val = obser[bin];
    // printf(" %f \n", val);
    // Write the data to the hyperslab
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, memspace_id, dataspace_id, H5P_DEFAULT, &val);
    if (status < 0) {
        printf("Error writing data\n");
    }

    // Close resources
    H5Sclose(memspace_id);
    H5Sclose(dataspace_id);
    H5Dclose(dataset_id);
    H5Fclose(file_id);


}


/**
 * @brief setter for scalar observables
*/
void Observable::set_scal_obser(const double& val, const int& bin){
    obser[bin]= val;
}
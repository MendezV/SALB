#ifndef INPUT_H
#define INPUT_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <hdf5.h>

#define PARAMS "parameters.txt"
#define DATA_FILE "data.hdf5"
#define THERMALIZATION_FILE "thermalization.out"

/*
generic namespace for simulation parameters 
*/

namespace SimConfig {

    extern std::string lat_type;
    extern int Lx;
    extern int Ly;
    extern int Lz;
    extern int n_therm_sweeps;
    extern int n_sweeps;
    extern int n_bins;
    extern int max_order;
    extern double beta;    
}

/*
namespace for model variable
*/

namespace ModelConfig {

    extern std::string model;

}

/*
namespaces for specific models 
*/

namespace HeisenbergConfig {

    extern double J;    

}

void loadSimConfig(const std::string& filename); //populating namespace
void loadModelConfig(const std::string& filename); //populating namespace
int create_hdf5_file_if_not_exists(); //creates the data file if it does not exist

//recording thermalization steps
int create_thermalization_log();
void print_thermalization_adjust(const int& sweep, const int& new_max_order);
#endif
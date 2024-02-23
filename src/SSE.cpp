#include <iostream>
#include <random>
#include <stdio.h>

#include "input_params.h"
#include "heisenberg.h"


void printarr(int* arr, const int& size);
int main(){

    std::mt19937 gen(0);
    bool adjusted_order;

    loadSimConfig(PARAMS);
    loadModelConfig(PARAMS);
    int created_hdf5 = create_hdf5_file_if_not_exists();
    Heisenberg model = Heisenberg(gen);


    // thermalization
    for(int sweep=0; sweep<SimConfig::n_therm_sweeps; sweep++){
        model.diagonal_update();
        model.conf.generate_vertex_list(model.lat.bond_list, model.lat.n_bonds);
        model.operator_loop_update();
        adjusted_order = model.conf.adjust_max_order();
        if(adjusted_order) print_thermalization_adjust(sweep, model.conf.n_max_ops);
    }

    // simulation
    for(int bin=0; bin<SimConfig::n_bins; bin++){
        for(int sweep=0; sweep<SimConfig::n_sweeps; sweep++){
            model.diagonal_update();
            model.conf.generate_vertex_list(model.lat.bond_list, model.lat.n_bonds);
            model.operator_loop_update();
        }
        model.measure_observables(bin);
    }
    
    return 0;

}


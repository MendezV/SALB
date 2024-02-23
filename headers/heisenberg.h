#ifndef HEISENBERG_H
#define HEISENBERG_H

#include <random>
#include <algorithm>
#include "lattice.h"
#include "configurations.h"
#include "input_params.h"
#include "observable.h"


/**
 * @brief Heisenberg class has specific update methods for the heisenberg model
 * 
 * 
 */
struct Heisenberg
{
    
    //parameters
    double beta;
    double J;
    int n_bins;
    Lattice lat;
    Configs conf;
    
    double diag_matrix_element;
    double add_prob_fac;
    double remove_prob_fac;
    

    //observables
    Observable energy;
    Observable magnetization;
    Observable staggered_magnetization;
    Observable heat_capacity;



    Heisenberg(std::mt19937& generator);  
    ~Heisenberg();

    //update routines
    void diagonal_update();
    void loop_flip(const int& vertex_0);
    void loop_reset(const int& vertex_0);
    void update_spin_after_loop();
    void operator_loop_update();

    //observable routines
    void measure_observables(const int& bin);
    double compute_magnetization();
    double compute_staggered_magnetization();

};

#endif
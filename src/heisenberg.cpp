#include "heisenberg.h"

/**
 * @brief Construct a new Heisenberg:: Heisenberg object
 * 
 * @param generator random number generator
 */
Heisenberg::Heisenberg(std::mt19937& generator)
    :lat(SimConfig::lat_type,SimConfig::Lx,SimConfig::Ly,SimConfig::Lz),conf(lat.n_sites, SimConfig::max_order, generator),
    //observable definitions
    energy("energy", "scalar"),
    magnetization("magnetization", "scalar"),
    staggered_magnetization("staggered_magnetization", "scalar"),
    heat_capacity("heat_capacity", "scalar")
{   
    beta = SimConfig::beta;
    J = HeisenbergConfig::J;
    diag_matrix_element = J/2;
    add_prob_fac = beta * diag_matrix_element * lat.n_bonds;
    remove_prob_fac = 1. / (beta * diag_matrix_element * lat.n_bonds);


}

Heisenberg::~Heisenberg(){
    
}

/**
 * @brief updates the configurations (spin and op) by going order by order and either
 * 1) adding a diagonal operator
 * 2) removing a diagonal operator
 * 3) flipping spins if an off diagonal operator was encountered
 */
void Heisenberg::diagonal_update(){

    int bond,s1,s2,operat;
    //random distributions for the update
    std::uniform_int_distribution<int>  rand_bond(0, lat.n_bonds);
    std::uniform_real_distribution<double>  rand_unif(0, 1);

    //looping over orders in the expansion
    for(int order=0; order<conf.n_max_ops; order++){

        operat = conf.op_string[order]; //operator at this order

        if(operat==0) //if there is no operator we consider adding a diagonal operator
        {
            bond = rand_bond(conf.gen); //randomly select a bond in the lattice
            s1 = lat.bond_list[bond]; //first site attached to the bond
            s2 = lat.bond_list[bond+lat.n_bonds]; //second site attached to the bond

            //if nearby spins are antiparallel, this bond is a candidate for adding a diagonal operator
            if(conf.spin_conf[s1]*conf.spin_conf[s2]<0){

                // adding a diag op with probability determined by add_prob
                if(rand_unif(conf.gen)*(conf.n_max_ops-conf.n_H_ops)<add_prob_fac){
                    conf.insert_diag_op(order, bond);

                }
            }
        }
        else if (operat%2 ==0) //if there is a diagonal operator we consider removing it
        {   
            // removing a diag op with probability determined by remove_prob
            if(rand_unif(conf.gen)<remove_prob_fac*(conf.n_max_ops-conf.n_H_ops+1)){
                    conf.remove_op(order);
                }
        }
        else{ // there is an off diagonal operator here so both spins are flipped
            bond = operat/2 -1;
            s1 = lat.bond_list[bond];
            s2 = lat.bond_list[bond+lat.n_bonds];
            conf.flip_spin(s1);
            conf.flip_spin(s2);
        }
        
    }
}

/**
 * @brief construct and flip a loop (diagonal to off-diagonal ops and vise versa), marks locations that have been 
 * flipped for updating the spin configuration at the end of the loop update
 * 
 * @param vertex_0 starting point of the loop
 */
void Heisenberg::loop_flip(const int& vertex_0){

    int vertex = vertex_0;
    int order, vertex_prime;

    do
    {   
        conf.mark_flipped_vertex(vertex);
        order = vertex/4;
        conf.flip_operator(order);
        vertex_prime =  conf.next_vert(vertex);
        vertex = conf.vertex_list[vertex_prime];
        conf.mark_flipped_vertex(vertex_prime);
        break;

    } while (vertex != vertex_0);
    

}

/**
 * @brief construct a loop and reset the vertex list along the loop without flipping operators
 * 
 * @param vertex_0 starting point of the loop
 */

void Heisenberg::loop_reset(const int& vertex_0){

    int vertex = vertex_0;
    int order, vertex_prime;
    
    do
    {   
        conf.reset_vertex(vertex);
        vertex_prime =  conf.next_vert(vertex);
        vertex = conf.vertex_list[vertex_prime];
        conf.reset_vertex(vertex_prime);
        break;

    } while (vertex != vertex_0);

}

/**
 * @brief after performing a loop update the spin configuration has to be updated
 * to match the new set of operators
 * if there are sites that are not connected to any operator, we flip them with 50% prob
 * 
 */
void Heisenberg::update_spin_after_loop(){

    int vertex;
    std::uniform_real_distribution<double>  rand_unif(0, 1);

    for(int site = 0; site<lat.n_sites; site++){
        vertex = conf.first_spin_op[site];
        if(vertex<-1){
            // if no operator was connected, we flip the spin 50% of the time
            if(rand_unif(conf.gen)<0.5){
                conf.flip_spin(site);
            }
        }
        else{  //if there is an operator connected
            // if the operator connected was flipped diring the loop update
            // we flip the spin correspondingly
            if(conf.vertex_list[vertex]==-2){
                conf.flip_spin(site);
            }

        }
    }
}


/**
 * @brief loops constructed and flipped with 50% probability
 * can change the winding number of configurations
 */
void Heisenberg::operator_loop_update(){

    std::uniform_real_distribution<double>  rand_unif(0, 1);

    for(int vertex=0; vertex<4*conf.n_max_ops; vertex++){ // number of vertices is 4*conf.n_max_ops
        if(conf.vertex_list[vertex]>=0){

            // with 50% probability we flip the loop starting at this operator
            // and indicate that the loop was flipped by seting the vertex list value to -2
            // this will be used to flip the spins at the boundary by keeping track of which connections
            // where flipped
            if(rand_unif(conf.gen)<0.5){
                loop_reset(vertex);
            }
            else{
                loop_flip(vertex);
            }

        }
    }
    update_spin_after_loop();

}

void Heisenberg::measure_observables(const int& bin){
    
    double obser_val;

    obser_val = -conf.n_H_ops/beta;
    energy.set_scal_obser(obser_val,bin);

    obser_val = compute_magnetization();
    magnetization.set_scal_obser(obser_val,bin);

    obser_val = compute_staggered_magnetization();
    staggered_magnetization.set_scal_obser(obser_val,bin);

    obser_val = conf.n_H_ops*conf.n_H_ops - conf.n_H_ops;
    heat_capacity.set_scal_obser(obser_val,bin);

    energy.record_data_h5(bin);
    magnetization.record_data_h5(bin);
    staggered_magnetization.record_data_h5(bin);
    heat_capacity.record_data_h5(bin);

}

double Heisenberg::compute_magnetization(){
    double magnetization;

    magnetization  = 0 ;

    for(int site = 0 ; site< lat.n_sites; site++){
        magnetization += conf.spin_conf[site];
    }


    return magnetization;

}

double Heisenberg::compute_staggered_magnetization(){

    double stag_magnetization;
    int a[3];
    int parity_site;

    stag_magnetization  = 0 ;
    
    for(int site = 0 ; site< lat.n_sites; site++){
        lat.index_to_ijk(site, a);
        parity_site = 2 * ( (a[0]+a[1]+a[2])%2 ) - 1 ; //sublattices labeled by whether the sum of coords is even or odd fo bipartite lat
        stag_magnetization += parity_site * conf.spin_conf[site];
    }

    return stag_magnetization;
}
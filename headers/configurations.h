#include <random>
#include <algorithm>

#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

struct Configs
{

    // CONVENTIONS overall 
    // if the maximum expansion order is max_order, there are max_order+1 = n_max_ops max operator insertions (zeroth order counts)
    // there is at most one operator per order and each operator has 4 legs, so we can label all possible legs of operators in the system 
    // using an index n that ranges from 0 to 4*N_max_ops -1. for labeling these vertex legs we use the convention that n = 4*p + l, where p is the order
    // in the expansion and l = 0,1,2,3 labels the vertex leg with Sandviks convention

    // convention for op_string:
    // each operator can be one of two types this will be encoded by the parity in op_string, 
    // the value of op string will be related to the bond of the operator 2(b+1) for diagonal and 2(b+1)+1 for off-diagonal
    // the bonds are labeled in the lattice class starting from zero, which then requires to use b+1 to avoid zeros when an operator is present

    //convention for spin_op lists:
    //the value will be given by the vertex leg that corresponds to the first or last connection of the spin at a given site, array indexed by site                             

    //convention for the vertex list: 
    // given the overall convention for labeling vertices, this list can be thought as a 4xn_max_ops sized list where each index represents a vertex leg in the system
    // and the entry is the vertex leg in the system to which there is a connection to. -1 is the convention if there is no operator inserted at this order at all
    
    int n_sites; //number of lattice sites
    int n_H_ops; //number of Hamiltonian insertions
    int max_order; //maximum order of the expansion
    int n_max_ops; //number of operators max_order + 1
    int n_vertices; //4*n_max_ops

    //random number generator
    std::mt19937& gen;

    
    //the spin configuration 
    int* spin_conf;

    //the operator and vertex lists
    int* op_string; //initialized to 0 size of the number of operators in the expansion
    int* first_spin_op; 
    int* last_spin_op;
    int* vertex_list;

    //constructors
    Configs( std::mt19937& generator );
    Configs(const int& Nsites, const int& max_order, std::mt19937& generator );
    ~Configs(); //destructor

    //initializer for arrays
    void initialize_val(int* arr, const int& size, const int& val);
    void initialize_rand(int* arr, const int& size);

    //adjust order in warmup phase
    bool adjust_max_order();

    //construct vertex list
    void generate_vertex_list(int* bond_list, int n_bonds);

    //methods that interface with the struct
    void flip_spin(const int& site);
    void insert_diag_op(const int& order, const int& bond);
    void remove_op(const int& order);
    void flip_operator(const int& order);
    void mark_flipped_vertex(const int& vertex);
    void reset_vertex(const int& vertex);
    int next_vert(int vertex);
    
};

#endif
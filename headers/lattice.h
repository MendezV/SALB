#include <string>

#ifndef LATTICE_H
#define LATTICE_H


struct Lattice
{

    std::string latt_type;

    int L1;
    int L2;
    int L3;
    int n_orb;
    int n_sites;
    int z; //cardinality of the lattice /2
    int n_bonds;
    // careful with pointers copy constructor
    int* bond_list; // entries 

    double* a1;
    double* a2;
    double* a3;
    

    Lattice(const std::string& latt_type, const int& Lx ,const int& Ly, const int& Lz); //default
    ~Lattice(); 

    /*
    ***********************
    INDEX METHODS 
    ***********************
    */
    inline int ijk_to_index(int& i, int& j, int& k) const;
    void index_to_ijk(int& index, int* a) const;

    /*
    ***********************
    FILLNG METHODS FOR BOND ARRAYS
    ***********************
    */
    void fill_bonds_cubic();
    
};

#endif
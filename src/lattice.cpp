#include "lattice.h"

/*
***********************
CONSTRUCTORS 
***********************
*/

Lattice::Lattice(const std::string& latt_type_in, const int& L1_in ,const int& L2_in, const int& L3_in)
    :latt_type(latt_type_in),L1(L1_in),L2(L2_in),L3(L3_in)
    {   
        
        a1= new double[3];
        a2= new double[3];
        a3= new double[3];


        if (latt_type == "cubic")
        {   
            n_orb=1;
            n_sites=L1*L2*L3*n_orb;
            
            z = 3;
            n_bonds = z * n_sites ;
            bond_list = new int[2 * n_bonds]; //2 bc theres two spins attached to a bond 
            fill_bonds_cubic();

            a1[0]=1.0;
            a1[1]=0.0;
            a1[2]=0.0;
            
            a2[0]=0.0;
            a2[1]=1.0;
            a2[2]=0.0;

            a3[0]=0.0;
            a3[1]=0.0;
            a3[2]=1.0;
            
        }
        else
        {

            printf("Lattice not implemented");
            exit(EXIT_SUCCESS);
        }
    }


Lattice::~Lattice()
{

    delete[] a1;
    delete[] a2;
    delete[] a3;
    delete[] bond_list;

}

/*
***********************
INDEX METHODS 
***********************
*/
inline int Lattice::ijk_to_index(int& i, int& j, int& k) const{
    
    return k + (j + L2 * i) * L3  ;

}


void Lattice::index_to_ijk(int& index, int* a ) const{

    a[2] = index % L3;
    a[1] = ((index - a[2])/L3 ) % L2;
    a[0] = ((index - a[2])/L3 -a[1] )/L2;


}

/*
***********************
FILLNG METHODS FOR BOND ARRAYS
***********************
*/
void Lattice::fill_bonds_cubic(){

    int i,j,k,site;
    int inn,jnn,knn,site_nn;

    for( i=0; i<L1; i++){
        for( j=0; j<L2; j++){
            for( k=0; k<L3; k++){

                site = ijk_to_index(i,j,k);

                inn = (i+1)%L1;
                jnn = j;
                knn = k;
                
                site_nn = ijk_to_index(inn,jnn,knn);
                bond_list[site] = site;
                bond_list[site + n_bonds] = site_nn;


                inn = i;
                jnn = (j+1)%L2;
                knn = k;
                
                site_nn = ijk_to_index(inn,jnn,knn);
                bond_list[site + n_sites] = site;
                bond_list[site + n_sites + n_bonds] = site_nn;


                inn = i;
                jnn = j;
                knn = (k+1)%L3;
                
                site_nn = ijk_to_index(inn,jnn,knn);
                bond_list[site + 2*n_sites] = site;
                bond_list[site + 2*n_sites + n_bonds] = site_nn;

            }
        }
    }
}
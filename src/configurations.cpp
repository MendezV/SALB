#include "configurations.h"

/*
***********************
CONSTRUCTORS 
***********************
*/
Configs::Configs(std::mt19937& generator) :gen(generator)
{
}
Configs::Configs(const int& n_sites_in, const int& max_order_in, std::mt19937& generator)
:n_sites(n_sites_in),max_order(max_order_in),gen(generator)
{

    n_H_ops = 0;
    n_max_ops = max_order + 1;
    n_vertices = 4*n_max_ops;

    //spin_conf in z basis
    spin_conf = new int[n_sites];
    initialize_rand(spin_conf, n_sites);

    //op string definition
    op_string = new int[n_max_ops];
    initialize_val(op_string, n_max_ops, 0);

    //spin_ops definitions
    first_spin_op = new int[n_sites];
    initialize_val(first_spin_op, n_sites, -1);
    last_spin_op  = new int[n_sites];
    initialize_val(last_spin_op, n_sites, -1);

    //vertex list definition
    vertex_list   = new int[n_vertices];
    initialize_val(vertex_list, n_vertices, -1);
    
};

Configs::~Configs()
{

    delete[] spin_conf;
    delete[] op_string;
    delete[] first_spin_op;
    delete[] last_spin_op;
    delete[] vertex_list;

}

//initializer for arrays
void Configs::initialize_val(int* arr, const int& size, const int& val)
{
    for(int i=0; i<size; i++){
        arr[i] = val;
    }
}

void Configs::initialize_rand(int* arr, const int& size)
{

    std::bernoulli_distribution dist(0.5);
    for(int i=0; i<size; i++){
        arr[i] =  2 * dist(gen) - 1;
    }
}

//adjust order in warmup phase
//test: copying was carried out without issues
bool Configs::adjust_max_order()
{
    int max_order_new =  n_H_ops + n_H_ops/3; //rule of thumb for increasing the maximum order of the expansion
    int n_max_ops_new = max_order_new + 1;

    bool adjust = (max_order_new>max_order);
    // printf("%d %d %d %d\n",max_order_new, max_order, n_H_ops,adjust);
    if(adjust)
    {   
        //copy and expand op_string;
        int* op_string_alt = new int[n_max_ops];
        std::copy(op_string, op_string + n_max_ops, op_string_alt);

        //regenerating op_string and copying
        delete[] op_string;
        op_string = new int[n_max_ops_new];
        initialize_val(op_string, n_max_ops_new, 0);
        std::copy(op_string_alt, op_string_alt + n_max_ops, op_string);
        delete[] op_string_alt;

        // redefining attributes now that we expanded op_string
        max_order = max_order_new;
        n_max_ops = n_max_ops_new;
        n_vertices = 4*n_max_ops;

        // regenerating vertex list
        delete[] vertex_list;
        vertex_list   = new int[n_vertices];
        initialize_val(vertex_list, n_vertices, -1);

        
    }

    return adjust;
}

//construct vertex list
void Configs::generate_vertex_list(int* bond_list, int n_bonds){
    
    // generates the vertex list by going order by order in the expansion.
    // if an operator is found the vertex list is updated by recording the 
    // vertex legs attached to the spins in the bond where an operator was found.
    // first_spin_op is used to record the first time a site encounters an op 
    // for later use connecting accross the tine boundary. 
    // last_op is updated as we increase orders in the expansion to record the last order in which
    // a site was acted on by an operator. 

    //test: there are groups of 4 non(-1) contiguous values always in the list
    //test: values are mutual in the list and its index
    int bond, s1, s2, vertex_1, vertex_2, vertex_0;

    //initializing spin lists and vertex list
    initialize_val(first_spin_op, n_sites, -1);
    initialize_val(last_spin_op, n_sites, -1);
    initialize_val(vertex_list, n_vertices, -1);

    //looping over orders in the expansion
    for(int n=0;n<n_max_ops;n++){

        if(op_string[n]>0){

            //obtaining bond associated with the operator at order i
            bond = op_string[n]/2 - 1; //subtracting one due to the labeling convention of bonds in the lattice class

            //sites connected by bond b
            s1 = bond_list[bond];
            s2 = bond_list[bond + n_bonds];

            //last spin operator that was encountered by spin at site s1 and the vertex that was encountered
            vertex_1 = last_spin_op[s1];
            
            //last spin operator that was encountered by spin at site s2 and the vertex that was encountered
            vertex_2 = last_spin_op[s2];

            //first leg of the operator at order n
            vertex_0 = 4 * n;

            //  checking the connections and updating the liked vertex list
            // if there was a previous operator connected to site s1 
            if(vertex_1 !=-1){

                // mutual connection between the vertices
                vertex_list[vertex_1] = vertex_0;
                vertex_list[vertex_0] = vertex_1;

            }
            else{ // if there is no previous connection to site s1, this was the first one 

                // setting the first operator encountered by s1
                first_spin_op[s1] = vertex_0;
            }

            // if there was a previous operator connected to site s2 
            if(vertex_2 !=-1){

                // mutual connection between the vertices
                vertex_list[vertex_2] = vertex_0+1;
                vertex_list[vertex_0+1] = vertex_2;

            }
            else{ // if there is no previous connection to site s2, this was the first one

                // setting the first operator encountered by s2
                first_spin_op[s2] = vertex_0+1;

            }
            
            //since s1 and s2 encountered an operator at this order
            //we update the last_spin_op list for both of them
            last_spin_op[s1]=vertex_0+2;
            last_spin_op[s2]=vertex_0+3;


        }

        //linking accross the time boundary
        for(s1 = 0; s1<n_sites; s1++){

            vertex_1 =first_spin_op[s1];
            
            if(vertex_1 != -1){

                vertex_2=last_spin_op[s1]; //this has to be different from -1 due to periodic boundary conds

                // mutual connection between the vertices accross time boundary
                vertex_list[vertex_2]=vertex_1;
                vertex_list[vertex_1]=vertex_2;
            }

        }

    }

}


//methods that interface with the struct
void Configs::flip_spin(const int& site){
    spin_conf[site]=-spin_conf[site];
}

void Configs::insert_diag_op(const int& order, const int& bond){
    op_string[order] = 2 * ( bond + 1 );
    n_H_ops = n_H_ops + 1;
}

void Configs::remove_op(const int& order){
    //remove op only if there was an op to begin with
    if(op_string[order] >0){
    op_string[order] = 0;
    n_H_ops = n_H_ops - 1;
    }
    else{
        printf("trying to remove op that is not there \n");
    }
}

void Configs::flip_operator(const int& order){
    int current_op= op_string[order];
    op_string[order] = current_op^1; // bitwise flip changes even to odd (larger) and vise-versa

}

void Configs::mark_flipped_vertex(const int& vertex){
    vertex_list[vertex] = -2; // if the operator at this vertex was flipped we mark the list with -2

}

void Configs::reset_vertex(const int& vertex){
    vertex_list[vertex] = -1; // reseting the value of the vertex list 

}

int Configs::next_vert(int vertex){ //copies vertex for evaluation
    return vertex^1; //if I'm at op 4 n + l this changes l to the next leg 0 ->1, 1->0, 2->3, and 3->2 using bitwise op
}
#ifndef OBSER_H
#define OBSER_H


#include <string>
#include "input_params.h"



struct Observable
{
    const char * name;
    std::string type;
    int n_bins;
    double* obser;

    Observable();
    Observable(const char* name, std::string type);
    ~Observable();

    //io for observables
    void record_data_h5(const int& bin);
    void init_obser_scal_h5();
    void record_data_scal_h5(const int& bin);

    //setters
    void set_scal_obser(const double& val, const int& bin);

};

#endif
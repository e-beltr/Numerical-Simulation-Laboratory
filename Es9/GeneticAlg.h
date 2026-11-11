#pragma once 
#include <fstream>
#include <iostream>
#include <string>
#include <armadillo>

using namespace std;
using namespace arma;

class GeneticAlg {
     protected:     
          inline static int _n_cities = 0;
          inline static int _n_individuals = 0;
          inline static vec _p_muts = zeros<vec>(4);
          inline static int _n_gens = 0;
          inline static double _p_selec = 0.0;
          inline static double _p_cross = 0.0;
     public:
          static void load_input() {
               ifstream in("input.dat");
               if (!in) {
                    cerr << "Error opening input file!" << endl;
                    return;
               }
               _p_muts.set_size(4);
               string label;
               in >> label >> _n_cities;
               in >> label >> _n_individuals;
               in >> label >> _n_gens;
               in >> label >> _p_muts(0); 
               in >> label >> _p_muts(1); 
               in >> label >> _p_muts(2);
               in >> label >> _p_muts(3);
               in >> label >> _p_selec;
               in >> label >> _p_cross;
               in.close();     
          }
};



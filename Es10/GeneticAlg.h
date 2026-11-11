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
          inline static int _n_steps = 0;
          inline static int _n_migr = 0;
          inline static double _t_min = 0.0;
          inline static double _t_max = 0.0;
     public:
          static void load_input() {
               ifstream in("input.dat");
               if (!in) {
                    cerr << "Error opening input file!" << endl;
                    return;
               }
               string label;
               in >> label >> _n_cities;
               in >> label >> _n_steps;
               in >> label >> _n_migr;
               in >> label >> _t_min;
               in >> label >> _t_max;
               in.close();     
          }
          static int get_n_cities() { return _n_cities; }
          static int get_n_steps() { return _n_steps; }
          static int get_n_migr() { return _n_migr; }
          static double get_t_min() { return _t_min; }
          static double get_t_max() { return _t_max; }
          static void set_inputs(int c, int s, int m) {
               _n_cities = c;
               _n_steps = s;
               _n_migr = m;
          }
          static void set_temperature(double t_min, double t_max) {
               _t_min = t_min;
               _t_max = t_max;
          }
};



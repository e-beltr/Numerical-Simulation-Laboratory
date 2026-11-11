/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numbers>
#include "../../PRNG/random.h"
#include "Metro.h"

using namespace std;
 
int main (int argc, char *argv[]){

    Random rnd;
    rnd.Initialize();

    int M{100000}; // Total number of throws for the Monte Carlo simulation
    int N{1000};    // Number of blocks for Data Blocking

    Metro my_metro(M, N, rnd, 1.2, 1, 0.5);             // Create Metro object with a step size of 1.2, mu = 1 and sigma = 0.5
    my_metro.auto_set_delta();                          // Automatically adjust the step size to achieve an acceptance rate of ~50%
    my_metro.run_DB();                                  // Run the Data Blocking simulation 
    my_metro.save_dat("metro_1s_unif.dat", 0.0);        // Save results
    my_metro.sample_positions(1000000, "positions.dat");
    rnd.SaveSeed();
    return 0;
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

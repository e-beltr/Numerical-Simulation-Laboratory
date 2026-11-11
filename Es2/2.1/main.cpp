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
#include "Integrals.h"

using namespace std;
 
int main (int argc, char *argv[]){

    Random rnd;                                        // Instantiate the random number generator
    rnd.Initialize();                                  // Load seeds and primes from external files

    int M{100000};                                     // Total number of Monte Carlo steps (throws)
    int N{100};                                        // Total number of blocks for data blocking

    // POINT 1: Compute the integral using a uniform distribution
    UnifIntegral unif_int(M,N,rnd);                    // Create object with parameters and RNG reference
    unif_int.run_DB();                                 // Run simulation and perform data blocking analysis
    unif_int.save_dat("unif_int.dat", 1.);             // Save block averages and errors (target = 1.0)

    // POINT 2: Compute the integral using Importance Sampling
    ImportanceSampling IS_int(M,N,rnd);                // Create object for variance reduction method
    IS_int.run_DB();                                   // Run simulation using the non-uniform PDF
    IS_int.save_dat("IS_int.dat", 1.);                 // Save data to compare precision with Point 1

    rnd.SaveSeed();                                    // Save the current seed for future reproducibility
    return 0;                                          // Return 0 to indicate successful execution
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
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
#include "Buffon.h"

using namespace std;
 
int main (int argc, char *argv[]){

    Random rnd;                                          // Instantiate the random number generator
    rnd.Initialize();                                    // Initialize the generator reading Primes and seed.in files

    int M{100000000};                                       // Total number of random needle throws
    int N{100};                                          // Number of blocks for Data Blocking

    double L_buf{0.8};                                   // Length of the needle (must be strictly less than d)
    double d{1.};                                        // Distance between the horizontal lines

    Buffon buff(M, N, rnd, L_buf, d);                    // Create the Buffon DataBlocking object
    buff.run_DB();                                       // Execute the progressive Data Blocking simulation
    buff.save_dat("buff.dat", numbers::pi);              // Save results shifting the graph to center around exact pi

    rnd.SaveSeed();                                      // Save the RNG seed state for future reproducible runs
    return 0;                                            // End of program
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
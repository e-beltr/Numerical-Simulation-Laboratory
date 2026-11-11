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

    int M{1000000}; // Total number of throws for the Monte Carlo simulation
    int N{100};    // Number of blocks for Data Blocking

    Metro metro_1s_unif(M, N, rnd, 1.2, true, false);       // Create Metro object for 1s state with a step size of 1.2
    metro_1s_unif.auto_set_delta();                         // Automatically adjust the step size to achieve an acceptance rate of ~50%
    metro_1s_unif.run_DB();                                 // Run the Data Blocking simulation for the 1s state
    metro_1s_unif.save_dat("metro_1s_unif.dat", 1.5);       // Save results shifted to center around the exact value of 1.5 for the 1s state

    Metro metro_2p_unif(M, N, rnd, 3, false, false);        // Create Metro object for 2p state with a step size of 3
    metro_2p_unif.auto_set_delta();                         // Automatically adjust the step size to achieve an acceptance rate of ~50%
    metro_2p_unif.run_DB();                                 // Run the Data Blocking simulation for the 2p state
    metro_2p_unif.save_dat("metro_2p_unif.dat", 5);         // Save results shifted to center around the exact value of 5 for the 2p state     
    
    Metro metro_1s_gauss(M, N, rnd, 0.75, true, true);      // Create Metro object for 1s state with Gaussian proposal distribution
    metro_1s_gauss.auto_set_delta();                        // Automatically adjust the step size to achieve an acceptance rate of ~50%
    metro_1s_gauss.run_DB();                                // Run the Data Blocking simulation for the 1s state with Gaussian proposals
    metro_1s_gauss.save_dat("metro_1s_gauss.dat", 1.5);     // Save results shifted to center around the exact value of 1.5 for the 1s state  

    Metro metro_2p_gauss(M, N, rnd, 1.5, false, true);      // Create Metro object for 2p state with Gaussian proposal distribution
    metro_2p_gauss.auto_set_delta();                        // Automatically adjust the step size to achieve an acceptance rate of ~50%
    metro_2p_gauss.run_DB();                                // Run the Data Blocking simulation for the 2p state with Gaussian proposals
    metro_2p_gauss.save_dat("metro_2p_gauss.dat", 5);       // Save results shifted to center around the exact value of 5 for the 2p state


    //Far from the origin
    int equil_step{5000};

    Metro metro_1s_far(M, N, rnd, 1.2, true, false, true);        // Create Metro uniform object for 1s state starting far from the origin  
    metro_1s_far.set_start_position(100.0, 100.0, 100.0);      // Set initial position far from the origin to test convergence
    metro_1s_far.run_equilibration(equil_step, "OUTPUT/scatter_1s_far_unif.dat");     // Run the equilibration simulation for the 1s state starting far from the origin

    Metro metro_2p_far(M, N, rnd, 3, false, false, true);         // Create Metro uniform object for 2p state starting far from the origin
    metro_2p_far.set_start_position(100.0, 100.0, 100.0);      // Set initial position far from the origin to test convergence
    metro_2p_far.run_equilibration(equil_step, "OUTPUT/scatter_2p_far_unif.dat");     // Run the equilibration simulation for the 2p state starting far from the origin   
    
    Metro metro_1s_correl(M, N, rnd, 1.2, true, false, true);
    metro_1s_correl.run_equilibration(20000);
    metro_1s_correl.run_equilibration(M, "OUTPUT/1s_correl.dat");


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

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
#include "../PRNG/random.h"
#include "EuropeansOptions.h"

using namespace std;
 
int main (int argc, char *argv[]){

   Random rnd;                                           // Instantiate the random number generator
   rnd.Initialize();                                     // Initialize the generator reading Primes and seed.in files

   int M{100000};                                        // Total number of throws for the Monte Carlo simulation
   int N{100};                                           // Number of blocks for Data Blocking
   
   const double s_0{100.};                               // Initial asset price (Spot price)
   const double T{1.};                                   // Delivery time (Maturity)
   const double K{100.};                                 // Strike price
   const double rate{0.1};                               // Risk-free interest rate (10%)
   const double vol{0.25};                               // Volatility of the underlying asset

   const double exact_call{14.975790778311286};          // Exact analytical Black-Scholes price for the Call
   const double exact_put{5.4595325819072364};           // Exact analytical Black-Scholes price for the Put

   // PUNTO 1

   EuropeanOption call(M, N, s_0, T, K, rate, vol, rnd, true, false); // Create DataBlocking object for DIRECT Call
   call.run_DB();                                        // Execute progressive Data Blocking simulation
   call.save_dat("call.dat", 0);                // Save results shifted to center around the exact value

   EuropeanOption put(M, N, s_0,T,K,rate, vol, rnd, false, false);    // Create DataBlocking object for DIRECT Put
   put.run_DB();                                         // Execute progressive Data Blocking simulation
   put.save_dat("put.dat", 0);                   // Save results shifted to center around the exact value

   // PUNTO 2

   EuropeanOption call_discr(M, N, s_0, T, K, rate, vol, rnd, true, true); // Create DataBlocking object for DISCRETE Call
   call_discr.run_DB();                                  // Execute progressive Data Blocking simulation
   call_discr.save_dat("call_discr.dat", 0);    // Save results shifted to center around the exact value

   EuropeanOption put_discr(M, N, s_0, T, K, rate, vol, rnd, false, true); // Create DataBlocking object for DISCRETE Put
   put_discr.run_DB();                                   // Execute progressive Data Blocking simulation
   put_discr.save_dat("put_discr.dat", 0);       // Save results shifted to center around the exact value

   rnd.SaveSeed();                                       // Save the RNG seed state for future reproducible runs
   return 0;                                             // End of program
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
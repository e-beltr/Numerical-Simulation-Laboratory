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
#include <cmath>
#include <iomanip>
#include "../../PRNG/random.h"

using namespace std;
 
int main (int argc, char *argv[]){

   Random rnd;                                           // Instantiate the random number generator
   rnd.Initialize();                                     // Initialize the generator reading Primes and seed.in files

   string out{"OUTPUT/"};
   ofstream out_unif(out + "uniform.dat");                     // Output file for the Uniform distribution sums
   ofstream out_exp(out + "exponential.dat");                  // Output file for the Exponential distribution sums
   ofstream out_cauchy(out + "cauchy.dat");                    // Output file for the Cauchy-Lorentz distribution sums
   const int N_2[] = {1, 2, 10, 100};                    // Array containing the number of variables N to sum

   for (int i{}; i < 10000; i++) {                       // Loop for 10^4 realizations (to build the final histograms)
      for (int N : N_2) {                                // Loop over the different values of N (1, 2, 10, 100)
         double sum_unif{};                              // Accumulator for the Uniform random variables
         double sum_exp{};                               // Accumulator for the Exponential random variables
         double sum_caulo{};                             // Accumulator for the Cauchy random variables
         for (int j{}; j < N; j++) {                     // Inner loop to generate and sum N_throws random numbers
            sum_unif+=rnd.Rannyu();                      // Extract and add a Uniform number in [0,1)
            sum_exp+=rnd.Exp(1);                         // Extract and add an Exponential number (lambda=1)
            sum_caulo+=rnd.CauLo(0,1);                   // Extract and add a Cauchy-Lorentz number (mu=0, gamma=1)
         }
         out_unif << sum_unif/N << " ";                  // Write the sample mean (S_N) for Uniform to file
         out_exp << sum_exp/N << " ";                    // Write the sample mean (S_N) for Exponential to file
         out_cauchy << sum_caulo/N << " ";               // Write the sample mean (S_N) for Cauchy to file
      }
      out_unif << endl;                                  // Newline to separate the 10^4 independent realizations
      out_exp << endl;                                   // Newline to separate the 10^4 independent realizations
      out_cauchy << endl;                                // Newline to separate the 10^4 independent realizations
   }

   out_unif.close();                                     // Close Uniform output file securely
   out_exp.close();                                      // Close Exponential output file securely
   out_cauchy.close();                                   // Close Cauchy output file securely

   rnd.SaveSeed();                                       // Save the RNG seed state for future runs
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
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
#include "UniformTest.h"

using namespace std;
 
int main (int argc, char *argv[]){
    Random rnd;                                          // Instantiate the random number generator
    rnd.Initialize();                                    // Initialize the generator reading Primes and seed.in files

    int M{1000000};                                       // Total number of random throws
    int N{100};                                          // Number of blocks for Data Blocking

    // PUNTO 1
    double exp_mean{0.5};                                // Exact analytical expectation value for the mean
    UniformMean unif_mean(M, N, rnd);                    // Create the DataBlocking object for the mean
    unif_mean.run_DB();                                  // Execute the progressive Data Blocking simulation
    unif_mean.save_dat("unif_mean.dat", exp_mean);       // Save results shifting the graph to center around 0
    
    // PUNTO 2
    double exp_var{1./12.};                              // Exact analytical variance
    UniformVariance unif_var(M, N, rnd);                 // Create the DataBlocking object for the variance
    unif_var.run_DB();                                   // Execute the progressive Data Blocking simulation
    unif_var.save_dat("unif_var.dat", exp_var);          // Save results shifting the graph to center around 0

    // PUNTO 3
    int n{10000};                                        // Total number of throws for EACH Chi-Square evaluation
    int M_chi{100};                                      // Number of sub-intervals (bins) in [0,1)
    int expected{n/M_chi};                                    // Expected number of events per bin (10000/100 = 100)

    double sum_tot{};                                    // Accumulator for the total sum of Chi-Square values
    double sum_sq{};                                     // Accumulator for the total sum of squared Chi-Square values
    vector<double> chi (M_chi);                          // Vector to store the 100 different Chi-Square evaluations
    int attempts{100}; 
    string filename = "OUTPUT/chi2_" + to_string(attempts) + ".dat";
    ofstream out_chi(filename);                        // Output file stream for the Chi-Square scatter plot

            
    for (int l{}; l<attempts; l++) {                          // Loop to repeat the Chi-Square test 100 times
        vector<int> counts(M_chi,0);                     // Vector to store observed frequencies, automatically reset to 0
        
        for (int i{}; i<n; i++) {                        // Loop to throw 10000 random numbers for the current test
            double rand{rnd.Rannyu()};                   // Extract a uniform random number in [0,1)
            int bin{int(rand*M_chi)};                    // Map the number to its corresponding bin index (0 to 99)
            counts[bin]++;                               // Increment the counter for the matched bin
        }
        double sum_chi{};                                // Accumulator for the Chi-Square mathematical formula
        for (int h{}; h<M_chi; h++) {                    // Loop over all 100 bins to compute the sum
            sum_chi += (counts[h] - expected)*(counts[h] - expected); // Numerator: (Observed - Expected)^2
        }
        chi[l]=sum_chi/expected;                              // Final Chi-Square for step l: Sum((O-E)^2)/E
        out_chi << l + 1 << "\t" << chi[l] << endl;      // Write the current Chi-Square evaluation to the data file
        sum_tot+=chi[l];                                 // Add to the total sum for final statistics
        sum_sq+=chi[l]*chi[l];                           // Add to the total squared sum for final statistics
    }
    out_chi.close();                                     // Close the output file stream securely

    double mean_chi{sum_tot/attempts};                       // Calculate the average Chi-Square value over the 100 tests
    double mean_sq{sum_sq/attempts};                         // Calculate the average squared Chi-Square value
    double err{sqrt((mean_sq - mean_chi*mean_chi)/(attempts-1))}; // Calculate the statistical standard error (N-1 = 99)

    cout << "Final χ-squared: " << mean_chi << " +/- " << err << endl; // Print the final result to the terminal

    rnd.SaveSeed();                                      // Save the final RNG state for future reproducible runs
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

#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;                                         // Uses standard namespace

class DataBlocking {                                         // Base class for Data Blocking analysis
     protected:
          int M, N, L, steps;                                // Total throws (M), number of blocks (N), throws per block (L), time steps
          vector<vector<double>> sum_prog, err_prog;         // Matrices for progressive averages and progressive errors

          double error(double AV, double AV2, int n) {       // Function to compute statistical error
               if (n == 0) return 0.0;                       // Error is artificially set to 0 for the very first block
               else return sqrt((AV2 - AV * AV) / n);        // Standard deviation of the mean formula
          }

     public:
          DataBlocking(int m, int n, int n_steps)            // Constructor
               : M(m), N(n), L(m / n), steps(n_steps),       // Initialization list for integer parameters
                    sum_prog(n, vector<double>(n_steps, 0.0)), // Allocates and initializes the progressive sum matrix to 0
                    err_prog(n, vector<double>(n_steps, 0.0)) {} // Allocates and initializes the progressive error matrix to 0
                    
          virtual ~DataBlocking() {}                         // Virtual destructor for safe inheritance

          virtual vector<double> simulate_block() = 0;       // Pure virtual method: child classes MUST implement the physics here

          void run_DB() {                                    // Core method executing the Data Blocking algorithm
               vector<vector<double>> ave(N, vector<double>(steps, 0.0));       // Matrix storing the average of each single block
               vector<vector<double>> av2(N, vector<double>(steps, 0.0));       // Matrix storing the squared average of each block
               vector<vector<double>> su2_prog(N, vector<double>(steps, 0.0));  // Matrix storing progressive sums of squared averages

               for (int i = 0; i < N; i++) {                 // Step 1: Simulate all blocks
                    ave[i] = simulate_block();               // Run simulation for the i-th block and get its trajectory
                    for (int k = 0; k < steps; k++) {        // Loop over all time steps
                         av2[i][k] = ave[i][k] * ave[i][k];  // Square the result for each time step
                    }
               }

               for (int i = 0; i < N; i++) {                 // Step 2: Calculate progressive quantities up to block i
                    for (int j = 0; j < i + 1; j++) {        // Accumulate data from block 0 up to current block i
                         for (int k = 0; k < steps; k++) {   // Loop over all time steps
                              sum_prog[i][k] += ave[j][k];   // Accumulate block averages
                              su2_prog[i][k] += av2[j][k];   // Accumulate squared block averages
                         }
                    }
                    for (int k = 0; k < steps; k++) {        // Finalize progressive values for the current block i
                         sum_prog[i][k] /= (i + 1);          // Compute the progressive mean
                         su2_prog[i][k] /= (i + 1);          // Compute the progressive mean of the squares
                         err_prog[i][k] = error(sum_prog[i][k], su2_prog[i][k], i); // Evaluate statistical error
                    }
               }
          }

          void save_dat(const string& filename) {            // Saves final results to file
               string folder_name = "OUTPUT";                // Defines the output directory name
               string full_path = folder_name + "/" + filename; // Constructs the full file path

               ofstream out(full_path);                      // Opens file stream for writing
               if (out.is_open()) {                          // Checks if the file was opened successfully
                    out << fixed << setprecision(10);        // Sets fixed decimal precision for clean output files
                    for (int k = 0; k < steps; k++) {        // Loops over all time steps
                         out << (k + 1) << "\t" << sum_prog[N - 1][k] << "\t" << err_prog[N - 1][k] << endl; // Prints: step, final average, final error
                    }
                    out.close();                             // Closes the file stream
                    cout << "Dati salvati correttamente in " << full_path << endl; // Success message
               } else {
                    cerr << "ERRORE: Impossibile aprire il file per la scrittura!" << endl; // Error message if file fails to open
               }
          }
};
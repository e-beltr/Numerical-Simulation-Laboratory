#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

class DataBlocking {
     protected:
          int M, N, L;                               // Total throws (M), number of blocks (N), throws per block (L)
          vector<double> ave, sum_prog, err_prog;         // Vectors for progressive averages and progressive errors

          double error (const vector<double>& AV, const vector<double> &AV2, int n) { // Computes statistical error
               if (n==0) return 0.0;                 // Error is artificially set to 0 for the first block (n=0)
               else return sqrt(abs((AV2[n]-AV[n]*AV[n]))/n); // Standard deviation of the mean
          }

     public:
          DataBlocking(int m, int n): M(m), N(n), L(m/n), ave(n, 0.0), sum_prog(n, 0.0), err_prog(n, 0.0) {} // Constructor
          virtual ~DataBlocking() {}                 // Virtual destructor for safe inheritance

          virtual double simulate_block()=0;         // Pure virtual method: child classes MUST implement the physics here

          void run_DB() {                            // Core method executing the Data Blocking method
               fill(sum_prog.begin(), sum_prog.end(), 0.0);
               vector<double> av2(N,0.0);            // Vector storing the square of the average of each single block
               vector<double> su2_prog(N, 0.0);      // Vector storing the progressive sum of the squared averages

               double accum_sum = 0.0;
               double accum_su2 = 0.0;

               for (int i = 0; i < N; i++) {
                    ave[i] = simulate_block();
                    av2[i] = ave[i] * ave[i];

                    accum_sum += ave[i];
                    accum_su2 += av2[i];

                    sum_prog[i] = accum_sum / (i + 1);
                    su2_prog[i] = accum_su2 / (i + 1);
                    
                    err_prog[i] = error(sum_prog, su2_prog, i); 
               }
          }

          void save_dat(const string& filename, double trasl) { // Saves results to file, 'trasl' is the exact analytical value
               string folder_name = "OUTPUT";
               string full_path = folder_name + "/" + filename;

               ofstream out(full_path);
               if (out.is_open()) {
                    out << fixed << setprecision(10); // Set fixed decimal precision for clean output files
                    for (int i = 0; i < N; i++) {
                         out << (i+1) << "\t" << ave[i] - trasl << "\t" << sum_prog[i] -trasl << "\t" << err_prog[i] << endl; // Prints: total throws, shifted value, error
                    }
                    out.close();
                    cout << "Dati salvati correttamente in " << full_path << endl;
                    } else {
                    cerr << "ERRORE: Impossibile aprire il file per la scrittura!" << endl;
               }
          }
          double get_final_ave() {
               return sum_prog.back(); // Returns the final progressive average after all blocks
          }
          double get_final_err() {
               return err_prog.back(); // Returns the final progressive error after all blocks
          }
};
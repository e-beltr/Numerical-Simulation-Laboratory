#pragma once 
#include <numbers>
#include <cmath>
#include <algorithm>
#include "../../PRNG/random.h"
#include "../../PRNG/DataBlocking.h"
using namespace std;

class Metro : public DataBlocking {
     private:
          Random& rnd;                                          // Reference to random number generator
          double delta;                                         // Uniform transition step size
          bool is_100;                                          // True for 1s state, False for 2p state
          bool is_gauss;                                        // True for Gaussian proposal distribution, False for uniform
          double x,y,z;                                         // Current position of the walker
          string scatter_file;                                  // Name of the scatter file in order to save xyz

          double r (double x, double y, double z) {             // Helper function to compute radial distance
               return sqrt(x*x+y*y+z*z);
          }
          double acceptance (double x_old, double y_old, double z_old, double x_new, double y_new, double z_new) { // Calculates Metropolis acceptance ratio
               double ratio{};
               double r_old{r(x_old, y_old, z_old)};
               double r_new{r(x_new, y_new, z_new)};
               if (is_100) {                                    // Probability ratio for the 1s ground state
                    ratio=exp(-2*(r_new-r_old));
               } else {                                         // Probability ratio for the 2p excited state
                    if (z_old==0) ratio=1.0;                    // Accept 100% to escape the nodal plane (p=0)
                    else ratio=z_new*z_new/(z_old*z_old)*exp(-r_new+r_old);
               }
               return ratio; 
          } 
          bool attempt_step () {                                // Performs a single Metropolis trial step
               double x_new, y_new, z_new;
               if (is_gauss) {                                  // Gaussian proposal distribution
                    x_new = x + rnd.Gauss(0.0, delta);
                    y_new = y + rnd.Gauss(0.0, delta);
                    z_new = z + rnd.Gauss(0.0, delta);
               } else {                                         // Uniform proposal distribution
                    x_new = x + delta * rnd.Rannyu(-1.0, 1.0);
                    y_new = y + delta * rnd.Rannyu(-1.0, 1.0);
                    z_new = z + delta * rnd.Rannyu(-1.0, 1.0);
               }
               double q = acceptance(x, y, z, x_new, y_new, z_new); // Evaluate acceptance probability ratio
               double alpha = min(1.0, q);
               if (rnd.Rannyu() < alpha) {                      // Accept or reject the proposed move
                    x = x_new;                                  // Update x to the new position
                    y = y_new;                                  // Update y to the new position
                    z = z_new;                                  // Update z to the new position
                    return true;                                // Indicate that the move was accepted
               }
               return false;                                    // Indicate that the move was rejected
          }                                 
     public: 
          Metro(int m, int n, Random& rand, double step, bool is_100, bool is_gauss, bool block_rewrite = false) : DataBlocking(m,n), rnd(rand), delta(step), is_100(is_100), is_gauss(is_gauss) { // Constructor setup
               if(is_100) {x=0.0; y=0.0; z=0.0;}                // Start 1s at the origin (max probability)
               else {x=0.0; y=0.0; z=2.0;}                      // Start 2p at the maximum probability peak 
               string orbital = is_100 ? "1s" : "2p";
               string method = is_gauss ? "gauss" : "unif";
               scatter_file = "OUTPUT/scatter_" + orbital + "_" + method + ".dat";
               
               if (!block_rewrite) {
                    ofstream reset(scatter_file);                    
                    reset.close();                  
               }           
          } 
          double simulate_block() override {                    // Performs one data block of Metropolis steps
               double sum{};                                    // Accumulator for the radial distances
               ofstream out(scatter_file, ios::app);
               for (int j{}; j<L; j++) {                        // Loop over all steps in the current block
                    attempt_step();                             // Perform a Metropolis trial step
                    sum += r(x, y, z);                          // Accumulate the current radius (whether moved or not)
                    if (j%50==0 && out.is_open()) out << x << "\t" << y << "\t" << z << "\n";  // Save scatter data every 50 steps for visualization
               }
               out.close();
               return sum/L;                                    // Return the block average
          }

          void auto_set_delta() {                               // Optional method to auto-tune the step size for ~50% acceptance
               double target_acc = 0.5;
               double tolerance = 0.02;
               double acc_rate = 0.0;
               int trial_steps = 1000;
               int attempt = 0;
               int max_attempts = 100;
               while (attempt < max_attempts) {
                    int acc = 0;
                    for (int j = 0; j < trial_steps; j++) {
                         if (attempt_step()) acc++;
                    }
                    acc_rate = double(acc) / trial_steps;
                    if (abs(acc_rate - target_acc) < tolerance) {
                         cout << "Auto-tuning complete: delta = " << delta << ", acceptance rate = " << acc_rate * 100 << "%" << endl;
                         return;                                // Desired acceptance rate achieved, exit the method
                    }
                    if (acc_rate < target_acc) delta *= 0.9;    // Decrease step size if acceptance is too low
                    else delta *= 1.1;                          // Increase step size if acceptance is too high
                    attempt++;
               }
                    
               cout << "Max attempts reached without achieving target acceptance rate. Final delta = " << delta << ", acceptance rate = " << acc_rate * 100 << "%" << endl;
          }
          
          void set_start_position(double new_x, double new_y, double new_z) { // Forces a custom starting position (useful for equilibration tests)
               x = new_x;                                       // Override the internal x coordinate
               y = new_y;                                       // Override the internal y coordinate
               z = new_z;                                       // Override the internal z coordinate
          }
          void run_equilibration(int steps, std::string filename = "") {
               if (filename.empty()) {
                    for(int i = 0; i < steps; i++) {
                         attempt_step();
                    }
               } else {
                    ofstream eq_out(filename);
                    for(int i = 0; i < steps; i++) {
                         attempt_step();                         // Metropolis move 
                         double current_r = r(x, y, z);          // Radius calculation
                         eq_out << i << "\t" << x << "\t" << y << "\t" << z << "\t" << current_r << "\n";
                    }
                    eq_out.close();
               }
          }
};
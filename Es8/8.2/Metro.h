#pragma once 
#include <numbers>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../../PRNG/random.h"
#include "../../PRNG/DataBlocking.h"
using namespace std;

class Metro : public DataBlocking {
     private:
          Random& rnd;                                          // Reference to random number generator
          double _delta;                                        // Uniform transition step size
          double _x, _mu, _sigma;
          int _accepted_steps = 0;                                  // Counter for accepted steps (for auto-tuning)

          double psi_t(double x) { // Evaluates the trial wavefunction at a given position
               double exp_1 = exp(-pow(x-_mu, 2)/(2*pow(_sigma,2)));
               double exp_2 = exp(-pow(x+_mu,2)/(2*pow(_sigma,2)));
               return exp_1 + exp_2;
          }
          double E_loc(double x) {
               double kappa = 1.0/(2*_sigma*_sigma*psi_t(x));
               double par_1 = pow(x-_mu,2);
               double par_2 = pow(x+_mu,2);
               double exp_1 = exp(-par_1/(2*_sigma*_sigma));
               double exp_2 = exp(-par_2/(2*_sigma*_sigma));
               double v_x = pow(x,4)-2.5*x*x;

               return -kappa*((par_1/(_sigma*_sigma)-1)*exp_1+(par_2/(_sigma*_sigma)-1)*exp_2) + v_x;
          }
          double ratio (double x_old, double x_new) { // Calculates Metropolis acceptance ratio
               return pow(psi_t(x_new),2)/pow(psi_t(x_old),2);
          }

          bool attempt_step () {                                // Performs a single Metropolis trial step
               double x_new;                                        // Uniform proposal distribution
                    x_new = _x + _delta * rnd.Rannyu(-1.0, 1.0);
             
               double q = ratio(_x, x_new);                     // Evaluate acceptance probability ratio
               double alpha = min(1.0, q);
               if (rnd.Rannyu() < alpha) {                      // Accept or reject the proposed move
                    _x = x_new;                                 // Update x to the new position
                    _accepted_steps++;                           // Increment the accepted steps counter
                    return true;                                // Indicate that the move was accepted
               }
               return false;                                    // Indicate that the move was rejected
          }                                 
     public: 
          Metro(int m, int n, Random& rand, double delta, double mu, double sigma) : DataBlocking(m,n), rnd(rand), _delta(delta), _mu(mu), _sigma(sigma) { // Constructor setup
               _x=_mu;                                          // Start at μ (max probability)
               
          } 
          double simulate_block() override {                    // Performs one data block of Metropolis steps
               double sum{};                                    // Accumulator
               for (int j{}; j<L; j++) {                        // Loop over all steps in the current block
                    attempt_step();                             // Perform a Metropolis trial step
                    sum += E_loc(_x);                           // Accumulate the current radius (whether moved or not)
               }
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
                         cout << "Auto-tuning complete: delta = " << _delta << ", acceptance rate = " << acc_rate * 100 << "%" << endl;
                         return;                                // Desired acceptance rate achieved, exit the method
                    }
                    if (acc_rate < target_acc) _delta *= 0.9;    // Decrease step size if acceptance is too low
                    else _delta *= 1.1;                          // Increase step size if acceptance is too high
                    attempt++;
               }
                    
               cout << "Max attempts reached without achieving target acceptance rate. Final delta = " << _delta << ", acceptance rate = " << acc_rate * 100 << "%" << endl;
          }
          
          void set_start_position(double new_x) { // Forces a custom starting position (useful for equilibration tests)
               _x = new_x;                                       // Override the internal x coordinate
          }
          void set_parameters(double new_mu, double new_sigma) { // Allows changing the parameters of the trial wavefunction on the fly
               _mu = new_mu;                                        // Update μ parameter
               _sigma = new_sigma;                                  // Update σ parameter
               _x = _mu;                                          // Reset position to the new μ (which is around the maximum of the probability distribution)
          }
          double get_mu() const { return _mu; }                   // Getter for the current μ parameter
          double get_sigma() const { return _sigma; }             // Getter for the current σ
          void reset_acc () { _accepted_steps = 0; }                     // Resets the accepted steps counter (useful for auto-tuning or diagnostics)
          double get_acc_rate() const { return (double)_accepted_steps/(N*L); }          // Getter for the number of accepted steps (useful for diagnostics)
          void equilibrate(int steps) { // Method to equilibrate the system by performing a number of Metropolis steps without recording data
               for (int i{}; i<steps; i++) {
                    attempt_step();                             // Perform a Metropolis trial step
               }
               reset_acc();
          }
          void sample_positions(int n_steps, const string& filename) { // Generates configurations for the histogram
               string folder_name = "OUTPUT";
               string full_path = folder_name + "/" + filename;
               ofstream out(full_path);
               
               if (out.is_open()) {
                    for (int i = 0; i < n_steps; i++) {
                         attempt_step();
                         out << _x << "\n";
                    }
                    out.close();
                    cout << "Posizioni campionate salvate in " << full_path << endl;
               } else {
                    cerr << "ERRORE: Impossibile aprire il file per le posizioni!" << endl;
               }
          }
};
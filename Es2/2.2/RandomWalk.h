#pragma once 
#include <vector>
#include <cmath>
#include <numbers>
#include "../../PRNG/random.h"
#include "DataBlocking.h"

using namespace std;

class discrete_RW : public DataBlocking {
     private: 
          Random& rnd;                                       // Reference to the pseudo-random number generator
          
     public:
          discrete_RW(int m, int n, int n_steps, Random& rand) : DataBlocking(m, n, n_steps), rnd(rand) {} // Constructor initializing base class and generator reference

          vector<double> simulate_block() override {         // Simulates one block of discrete random walks
               double pos[3];                                // Current 3D Cartesian coordinates (x, y, z)
               vector<double> acc_r2(steps);                 // Accumulator for squared distance at each time step
               vector<double> meas(steps);                   // Vector storing root-mean-square distance per time step

               acc_r2.assign(acc_r2.size(), 0.0);            // Resets the squared distance accumulator to zero
               for (int j{}; j<L; j++) {                     // Loops over all random walks within the single block
                    pos[0] = 0.0; pos[1] = 0.0; pos[2] = 0.0;// Resets walker position to the origin (0, 0, 0)
                    for (int k{}; k<steps; k++) {            // Iterates through time steps of the walk
                         int dir = int(rnd.Rannyu(0.0, 3.0));// Selects spatial direction uniformly (0: x, 1: y, 2: z)
                         double verso = (rnd.Rannyu() < 0.5) ? 1.0 : -1.0; // Chooses forward (+1) or backward (-1) step
                         pos[dir] += verso;                  // Updates position along the chosen coordinate axis
                         acc_r2[k] += pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]; // Accumulates squared distance r^2 at step k
                    }
               }
               for (int k{}; k<steps; k++) {                 // Evaluates the block-averaged quantities
                  meas[k] = sqrt(acc_r2[k]/L);               // Computes sqrt(<r^2>) for step k across the block
               }
               return meas;                                  // Returns the full trajectory for data blocking analysis
          }
};

class continuous_RW : public DataBlocking {
     private: 
          Random& rnd;                                       // Reference to the pseudo-random number generator
          
     public:
          continuous_RW(int m, int n, int n_steps, Random& rand) : DataBlocking(m, n, n_steps), rnd(rand) {} // Constructor initializing base class and generator reference

          vector<double> simulate_block() override {         // Simulates one block of continuous random walks
               double pos[3];                                // Current 3D Cartesian coordinates (x, y, z)
               vector<double> acc_r2(steps);                 // Accumulator for squared distance at each time step
               vector<double> meas(steps);                   // Vector storing root-mean-square distance per time step
               acc_r2.assign(acc_r2.size(), 0.0);            // Resets the squared distance accumulator to zero
               for (int j{}; j<L; j++) {                     // Loops over all random walks within the single block
                    pos[0] = 0.0; pos[1] = 0.0; pos[2] = 0.0;// Resets walker position to the origin (0, 0, 0)
                    for (int k{}; k<steps; k++) {            // Iterates through time steps of the walk
                    double phi{rnd.Rannyu(0, 2*numbers::pi)};// Samples azimuthal angle uniformly in [0, 2*pi)
                    double cos_theta{1-2*rnd.Rannyu()};      // Samples cos(theta) uniformly in [-1, 1] for solid angle isotropy
                    double sin_theta{sqrt(1-cos_theta*cos_theta)}; // Calculates sin(theta) from trigonometric identity
                    pos[0]+=sin_theta*cos(phi);              // Updates x coordinate by unit step projection
                    pos[1]+=sin_theta*sin(phi);              // Updates y coordinate by unit step projection
                    pos[2]+=cos_theta;                       // Updates z coordinate by unit step projection
                    acc_r2[k] += pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]; // Accumulates squared distance r^2 at step k
                    }
               }
               for (int k{}; k<steps; k++) {                 // Evaluates the block-averaged quantities
                  meas[k] = sqrt(acc_r2[k]/L);               // Computes sqrt(<r^2>) for step k across the block
               }
               return meas;                                  // Returns the full trajectory for data blocking analysis
          }
};
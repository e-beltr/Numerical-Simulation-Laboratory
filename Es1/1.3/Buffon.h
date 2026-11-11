#pragma once 
#include "../../PRNG/random.h"
#include "../../PRNG/DataBlocking.h"
using namespace std;

class Buffon : public DataBlocking {
     private:
          Random& rnd;                                               // Reference to the random number generator
          double L_buf, d;                                           // Needle length (L_buf) and distance between lines (d)
          
     public:
          Buffon(int m, int n, Random& rand, double L_buff, double d_buff) : DataBlocking(m,n), rnd(rand), L_buf(L_buff), d(d_buff) {}
          
          double simulate_block() override {                         // Simulates dropping L needles to estimate pi
               int hit{};                                            // Counter for the needles crossing a line
               for (int j{}; j<L; j++) {
                    double y_center= rnd.Rannyu(0.0, d/2.0);         // Distance from needle center to the closest line
                    double x_circ, y_circ, rad;                      // Variables for 2D rejection sampling
                    do {
                         x_circ=rnd.Rannyu();                        // Random x coordinate in [0,1)
                         y_circ=rnd.Rannyu();                        // Random y coordinate in [0,1)
                         rad=sqrt(x_circ*x_circ + y_circ*y_circ);    // Distance from the origin
                    } while (rad>1.0);                               // Reject points outside the unit quarter-circle
                    double sin_theta= y_circ/rad;                    // Compute sine of the angle purely geometrically (NO pi used!)

                    if (y_center<=(L_buf/2.0)*sin_theta) hit++;      // Check intersection condition
               }
               double pi=2.0*L_buf*L/double(hit)/d;                  // Compute Buffon's pi estimate for this specific block
               return pi;                                            // Return the value to the Mother class for progressive averaging
          }
};
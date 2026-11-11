#pragma once 
#include "../PRNG/random.h"
#include "../PRNG/DataBlocking.h"
#include <algorithm>
#include <cmath>

using namespace std;

class EuropeanOption : public DataBlocking {
     private:
          double S_0, T, K, rate, vol;                               // Financial parameters: spot price, time, strike, rate, volatility
          Random& rnd;                                               // Reference to the random number generator
          bool is_call;                                              // Flag: true for Call option, false for Put option
          bool is_discrete;                                          // Flag: true for 100-step path, false for direct jump
     
     public:
          EuropeanOption(int m, int n, double s_0, double t, double k, double r, double v, Random& rand, bool call, bool discrete)
          : DataBlocking(m,n), S_0(s_0), T(t), K(k), rate(r), vol(v), rnd(rand), is_call(call), is_discrete(discrete) {} // Constructor

          double simulate_block() override {                         // Simulates one block of option pricing
               double sum{};                                         // Accumulator for the discounted payoffs
               double discount{exp(-rate*T)};                        // Pre-calculated discount factor for optimization
               double drift{rate-0.5*vol*vol};                       // Pre-calculated drift factor for optimization

               for (int j{}; j<L; j++) {                             // Loop over L throws per block
                    double s_t = S_0;                                // Initial asset price
                    if(!is_discrete) {                               // DIRECT path generation
                         double z{rnd.Gauss(0,1)};                   // Extract standard normal variable
                         s_t=S_0*exp((drift*T+vol*z*sqrt(T)));       // Jump directly to maturity T
                    } else {                                         // DISCRETE path generation (100 steps)
                         double dt{T/100.0};                         // Time step width
                         for (int k{}; k<100; k++) {                 // Loop over 100 time intervals
                              double z{rnd.Gauss(0,1)};              // Extract standard normal variable for current step
                              s_t*=exp(drift*dt+vol*z*sqrt(dt));     // Evolve price by one time step dt
                         }
                    }

                    if (is_call) sum+=discount*max(0.0, s_t-K);      // Accumulate discounted Call payoff
                    else         sum+=discount*max(0.0, K-s_t);      // Accumulate discounted Put payoff
               }
               return sum/L;                                         // Return the block average
          }

};
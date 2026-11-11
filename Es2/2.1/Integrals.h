#pragma once 
#include <numbers>
#include "random.h"
#include "../../PRNG/DataBlocking.h"
using namespace std;

class UnifIntegral : public DataBlocking {
     private: 
          Random& rnd;                                       // Reference to the random number generator

     public:
          UnifIntegral(int m, int n, Random& rand) : DataBlocking(m,n), rnd(rand) {} // Constructor initializing base class and RNG
          double simulate_block() override {                 // Simulates one block for the uniform sampling integration
               double sum{};                                 // Accumulator for the integral evaluation
               for (int j{}; j<L; j++) {
                    sum += numbers::pi/2*cos(numbers::pi*rnd.Rannyu()/2); // Evaluates f(x) with uniform x in [0,1)
               }
               return sum/L;                                 // Returns the block average
          }
};

class ImportanceSampling : public DataBlocking {
     private:
          Random& rnd;                                       // Reference to the random number generator

     public: 
          ImportanceSampling(int m, int n, Random& rand) : DataBlocking(m,n), rnd(rand) {} // Constructor initializing base class and RNG
          double simulate_block() override {                 // Simulates one block for importance sampling integration
               double sum{};                                 // Accumulator for the integral evaluation
               for (int j{}; j<L; j++) {
                    double x{1-sqrt(1-rnd.Rannyu())};        // Extracts x from p(x) = 2(1-x) using the inversion method
                    sum+=numbers::pi/4.0*cos(numbers::pi/2*x)/(1-x); // Evaluates f(x)/p(x) at the sampled point x
               }
               return sum/L;                                 // Returns the block average
          }
};
#pragma once 
#include "random.h"
#include "../../PRNG/DataBlocking.h"
using namespace std;

class UniformMean : public DataBlocking {
     private:
          Random& rnd;                                       // Reference to the random number generator
          
     public:
          UniformMean(int m, int n, Random& rand) : DataBlocking(m, n), rnd(rand) {} // Constructor initializing base class and RNG

          double simulate_block() override {                 // Computes the integral of r for the Mean
               double sum{};
               for (int j{}; j<L; j++) {
                    sum += rnd.Rannyu();                     // Extracts a uniform random number in [0,1)
               }
               return sum/L;                                 // Returns the average of the block
          }
};

class UniformVariance : public DataBlocking {
     private:
          Random& rnd;                                       // Reference to the random number generator

     public:
          UniformVariance(int m, int n, Random& rand) : DataBlocking(m, n), rnd(rand) {} // Constructor initializing base class and RNG

          double simulate_block() override {                 // Computes the integral of (r-0.5)^2 for the Variance
               double sum{};
               for (int j{}; j<L; j++) {
                    double r{rnd.Rannyu()};                  // Extracts a uniform random number in [0,1)
                    sum += (r-0.5)*(r-0.5);                  // Accumulates the squared difference from 0.5
               }
               return sum/L;                                 // Returns the variance of the block
          }

};
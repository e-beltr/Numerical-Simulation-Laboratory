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
#include <cmath>
#include <cstdlib>
#include <numbers>
#include <string>
#include "random.h"

using namespace std;

Random :: Random(){}
// Default constructor, does not perform any action

Random :: ~Random(){}
// Default destructor, does not perform any action

void Random :: SaveSeed(){
   // This function saves the current state of the random number generator to a file "seed.out"
   ofstream WriteSeed;
   WriteSeed.open("OUTPUT/seed.out");
   if (WriteSeed.is_open()){
      WriteSeed << "RANDOMSEED	" << l1 << " " << l2 << " " << l3 << " " << l4 << endl;;
   } else cerr << "PROBLEM: Unable to open random.out" << endl;
  WriteSeed.close();
  return;
}

void Random :: Initialize(){
   int seed[4];
   int p1, p2;

   // 1. APERTURA PRIMES
   ifstream Primes("../PRNG/Primes");
   if (!Primes.is_open()) { Primes.clear(); Primes.open("../../PRNG/Primes"); }
   if (!Primes.is_open()) { Primes.clear(); Primes.open("Primes"); }
   if (!Primes.is_open()) {
      cerr << "PROBLEM: Unable to open Primes" << endl;
      exit(EXIT_FAILURE);
   }
   Primes >> p1 >> p2;
   Primes.close();

   // 2. APERTURA SEED.IN
   ifstream input("../PRNG/seed.in");
   if (!input.is_open()) { input.clear(); input.open("../../PRNG/seed.in"); }
   if (!input.is_open()) { input.clear(); input.open("seed.in"); }
   if (!input.is_open()) {
      cerr << "PROBLEM: Unable to open seed.in!" << endl;
      exit(EXIT_FAILURE);
   }

   string property;
   while (!input.eof()) {
      input >> property;
      if (property == "RANDOMSEED") {
         input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
         SetRandom(seed, p1, p2);
      }
   }
   input.close();
}

void Random :: Initialize(int shift_prime){
   //This function intialize the seed reading from files
   int seed[4];
   int p1, p2;
   ifstream Primes("../PRNG/Primes");
   if (Primes.is_open()){
      for (int i = 0; i < shift_prime; i++) {
          Primes >> p1 >> p2; 
      }
      Primes >> p1 >> p2;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   ifstream input("../PRNG/seed.in");
   string property;
   if (input.is_open()){
      while ( !input.eof() ){
         input >> property;
         if( property == "RANDOMSEED" ){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            SetRandom(seed,p1,p2);
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;
}


double Random :: Gauss(double mean, double sigma) {
   // This function generates a random number from a Gaussian distribution with given mean and sigma
   double s=Rannyu();
   double t=Rannyu();
   double x=sqrt(-2.*log(1.-s))*cos(2.*M_PI*t);
   return mean + x * sigma;
}

double Random :: Exp(double lambda) {
   // This function generates a random number from an exponential distribution with given λ
   double y=Rannyu();
   return -log(1-y)/lambda;
}


double Random :: CauLo(double mu, double gamma) {
   // This function generates a random number from a Cauchy-Lorentz distribution with given μ and Γ
   double y=Rannyu();
   return mu + gamma*tan(numbers::pi*(y-0.5));
}

double Random :: Rannyu(double min, double max) {
   // This function generates a random number in the range [min, max)
   return min+(max-min)*Rannyu();
}

double Random :: Rannyu(void){
  // This function generates a random number in the range [0,1)
  const double twom12=0.000244140625;
  int i1,i2,i3,i4;
  double r;

  i1 = l1*m4 + l2*m3 + l3*m2 + l4*m1 + n1;
  i2 = l2*m4 + l3*m3 + l4*m2 + n2;
  i3 = l3*m4 + l4*m3 + n3;
  i4 = l4*m4 + n4;
  l4 = i4%4096;
  i3 = i3 + i4/4096;
  l3 = i3%4096;
  i2 = i2 + i3/4096;
  l2 = i2%4096;
  l1 = (i1 + i2/4096)%4096;
  r=twom12*(l1+twom12*(l2+twom12*(l3+twom12*(l4))));

  return r;
}

void Random :: SetRandom(int * s, int p1, int p2){
  // This function sets the seed and parameters of the random number generator
  m1 = 502;
  m2 = 1521;
  m3 = 4071;
  m4 = 2107;
  l1 = s[0];
  l2 = s[1];
  l3 = s[2];
  l4 = s[3];
  n1 = 0;
  n2 = 0;
  n3 = p1;
  n4 = p2;

  return;
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

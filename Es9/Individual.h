#pragma once 
#include <armadillo>
#include "../PRNG/random.h"
#include "GeneticAlg.h"
#include "Map.h"

using namespace std;
using namespace arma;

class Individual : public GeneticAlg {
     private:
      uvec _genes;
      double _fitness;

     public:
      
      Individual();
      Individual(const Individual &other);
      Individual& operator=(const Individual& other);
      void pair_perm(Random &rnd);
      void shift_mut(int contiguous_cities, int n_shift, Random &rnd);
      void m_perm(int m, Random &rnd);
      void inversion_mut(Random &rnd);
      void compute_fitness (Map &map);
      double get_fitness () const;
      bool check();
      int get_gene(int index) const;
      void set_gene(int index, int value);
      void standardize_dir();
      double get_p_mut(int mutation_type);
};



inline Individual::Individual() {
   _genes = regspace<uvec>(0, _n_cities - 1);
}

inline Individual::Individual(const Individual &other) { 
   _genes = other._genes;
   _fitness = other._fitness;
}

inline Individual& Individual::operator=(const Individual& other) {
  if (this != &other) { 
      _genes = other._genes;
      _fitness = other._fitness;
  }
  return *this;
}

inline void Individual::pair_perm(Random &rnd) {
   int i,j;
   i = int(rnd.Rannyu(1.0, double(_n_cities)));
   do {
      j = int(rnd.Rannyu(1.0, double(_n_cities)));
   } while (j==i);
   swap(_genes(i), _genes(j));
}

inline void Individual::shift_mut(int contiguous_cities, int n_shift, Random &rnd) {
   int L = _n_cities - 1;
   int start = int(rnd.Rannyu(1.0, double(_n_cities)));
   contiguous_cities = contiguous_cities % L;
   if (contiguous_cities==0) contiguous_cities = 1;
   n_shift = n_shift % (L - contiguous_cities +1);

   uvec mobile = _genes.subvec(1, _n_cities - 1);
   mobile = shift(mobile, (start - 1));

   uvec sub = mobile.subvec(0, contiguous_cities + n_shift -1);
   sub = shift(sub, contiguous_cities);
   mobile.subvec(0, contiguous_cities + n_shift - 1) = sub;
   mobile = shift(mobile, -(start-1));
   _genes.subvec(1, _n_cities -1) = mobile;
}

inline void Individual::m_perm(int m, Random &rnd) {
   int L = _n_cities - 1; 
   if (m > L/2) m = L / 2;
   if (m < 1) m = 1;

   int p1 = int(rnd.Rannyu(1.0, double(L - 2 * m + 2)));
   int p2 = int(rnd.Rannyu(double(p1 + m), double(_n_cities - m + 1)));

   uvec block_1 = _genes.subvec(p1, p1 + m - 1);
   uvec block_2 = _genes.subvec(p2, p2 + m - 1);

   _genes.subvec(p2, p2 + m - 1) = block_1;
   _genes.subvec(p1, p1 + m - 1) = block_2;
}

inline void Individual::inversion_mut(Random &rnd) {
   int p1 = int(rnd.Rannyu(1.0, double(_n_cities)));
   int p2 = int(rnd.Rannyu(1.0, double(_n_cities)));
   while (p1 == p2) {
       p2 = int(rnd.Rannyu(1.0, double(_n_cities)));
   }
   if (p1 > p2) swap(p1, p2);
   _genes.subvec(p1, p2) = arma::reverse(_genes.subvec(p1, p2));
}

inline void Individual::compute_fitness (Map &map) {
   double L = 0.0;
   for (int i = 0; i < _n_cities - 1; ++i) {
      L += map.get_distance(_genes(i), _genes(i + 1));
   }
   L += map.get_distance(_genes(_n_cities - 1), _genes(0));
   _fitness = L;
}

inline double Individual::get_fitness () const {
   return _fitness;
}

inline bool Individual::check() {
   if (_genes(0) != 0) return false;
   uvec sorted_genes = sort(_genes);
   for (int i = 0; i < _n_cities; ++i) {
      if (sorted_genes(i) != (unsigned int)i) return false;
   }
   return true;
}

inline int Individual::get_gene(int index) const {
   return _genes(index);
}

inline void Individual::set_gene(int index, int value) {
   _genes(index) = value;
}

inline void Individual::standardize_dir() {
   if (_genes(1) > _genes(_n_cities - 1)) {
      _genes.subvec(1, _n_cities - 1) = arma::reverse(_genes.subvec(1, _n_cities - 1));
   }
}

inline double Individual::get_p_mut(int mutation_type) {
   return _p_muts(mutation_type);
}
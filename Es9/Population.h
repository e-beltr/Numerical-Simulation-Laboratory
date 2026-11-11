#pragma once 
#include <armadillo>
#include <algorithm>
#include <utility>
#include <vector>
#include "Individual.h"
#include "../PRNG/random.h"
#include "GeneticAlg.h"

using namespace std;
using namespace arma;

class Population : public GeneticAlg {
    private:
        field<Individual> _individuals;
        Map& _map;
    
    public:
        // Dichiarazioni delle funzioni (il "sommario")
        Population(Map& inp_map);
        void initialize(Random &rnd);
        void sort_individuals();
        int select_individual(Random &rnd);
        pair<Individual, Individual> crossover(Individual const &parent_1, Individual const &parent_2, Random &rnd);
        void new_gen(Random &rnd);
        int get_n_gens() const;
        int get_n_individuals() const;
        double get_best_fitness() const;
        double compute_half_mean() const;
        void mutate_generation(Random &rnd);
        Individual get_best_individual() const;
};



Population::Population(Map& inp_map) : _map(inp_map), _individuals(_n_individuals) {}

inline void Population::initialize(Random &rnd) {
    Individual first_chrom;
    _individuals(0) = first_chrom;
    int n_shuffle = _n_cities*3;
    for (int i{}; i < _n_individuals; i++) {
        _individuals(i) = first_chrom;
        bool is_valid = false;
        do {
        for (int j{}; j< n_shuffle; j++ ){
            _individuals(i).pair_perm(rnd);
        }
        is_valid = _individuals(i).check();
        } while (is_valid == false);
    }
}

inline void Population::sort_individuals() {
    for (auto& ind : _individuals) {
        ind.compute_fitness(_map);
    }
    vector<int> indices(_n_individuals);
    for (int i = 0; i < _n_individuals; ++i) {
        indices[i] = i;
    }
    sort(indices.begin(), indices.end(), [this](int a, int b) {
        return _individuals(a).get_fitness() < _individuals(b).get_fitness();
    });
    field<Individual> sorted_individuals(_n_individuals);
    for (int i = 0; i < _n_individuals; ++i) {
        sorted_individuals(i) = _individuals(indices[i]);       
    }
    _individuals = sorted_individuals;
}



inline int Population::select_individual(Random &rnd) {
    double r = rnd.Rannyu();
    double p = _p_selec;
    int index = int(pow(r, p) * _n_individuals);
    return index;
}


inline pair<Individual, Individual> Population::crossover(Individual const &parent_1, Individual const &parent_2, Random &rnd) {
    Individual child_1(parent_1);
    Individual child_2(parent_2);
    int cut_index = int(rnd.Rannyu(1, _n_cities-1));

    int child_1_index = cut_index;
    for (int i{}; i < _n_cities; i++) {
        int candidate_city = parent_2.get_gene(i);
        bool already_in = false;
        for (int j{}; j < cut_index; j++) {
            if (candidate_city == child_1.get_gene(j)) {
                already_in = true;
                break;
            }
        }
        if (already_in == false) {
            child_1.set_gene(child_1_index, candidate_city);
            child_1_index ++;
        }
    }

    int child_2_index = cut_index;
    for (int i{}; i < _n_cities; i++) {
        int candidate_city = parent_1.get_gene(i);
        bool already_in = false;
        for (int j{}; j < cut_index; j++) {
            if (candidate_city == child_2.get_gene(j)) {
                already_in = true;
                break;
            }
        }
        if (already_in == false) {
            child_2.set_gene(child_2_index, candidate_city);
            child_2_index ++;
        }
    }
    return make_pair(child_1, child_2);
}


inline void Population::new_gen(Random &rnd) {
    arma::field<Individual> next_gen(_n_individuals);
    next_gen(0) = _individuals(0);
    next_gen(1) = _individuals(1);

    for (int i = 2; i < _n_individuals; i += 2) {
        int p1_index = select_individual(rnd);
        int p2_index;
        do {
            p2_index = select_individual(rnd);
        } while (p1_index == p2_index);
        pair<Individual, Individual> children;
        if (rnd.Rannyu() < _p_cross) {
            children = crossover(_individuals(p1_index), _individuals(p2_index), rnd);
        } else children = make_pair(_individuals(p1_index), _individuals(p2_index));
        
        children.first.standardize_dir();
        children.second.standardize_dir();
        next_gen(i) = children.first;
        if (i+1 < _n_individuals) next_gen(i+1) = children.second;
    }
    _individuals = next_gen;
}


inline int Population::get_n_gens() const {
    return _n_gens;
}

inline int Population::get_n_individuals() const {
    return _n_individuals;
}

inline double Population::get_best_fitness() const {
    return _individuals(0).get_fitness();
}

inline double Population:: compute_half_mean() const {
    double sum = 0.0;
    for (int i{}; i < _n_individuals/2; i++) {
        sum += _individuals(i).get_fitness();
    }
    return sum / (_n_individuals/2);
}

inline Individual Population::get_best_individual() const {
    return _individuals(0);
}

inline void Population::mutate_generation(Random &rnd) {
    for (int i=2; i < _n_individuals; i++) {
            if (rnd.Rannyu() < _individuals(i).get_p_mut(0)) {
                _individuals(i).pair_perm(rnd);
            }
            if (rnd.Rannyu() < _individuals(i).get_p_mut(1)) {
                _individuals(i).shift_mut(3, 1, rnd);
            }
            if (rnd.Rannyu() < _individuals(i).get_p_mut(2)) {
                _individuals(i).m_perm(3, rnd);
            }
            if (rnd.Rannyu() < _individuals(i).get_p_mut(3)) {
                _individuals(i).inversion_mut(rnd);
            }
        }
}

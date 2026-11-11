#pragma once 
#include <armadillo>
#include <numbers>
#include <fstream>
#include <string>
#include "../PRNG/random.h"
#include "GeneticAlg.h"

using namespace arma;

class Map : public GeneticAlg {
private:     
     mat _pos;           
     mat _distances; 
     bool _is_round;

public:
     Map(bool is_round) : _is_round(is_round) {}
     void generate_map(double side, Random &rnd) {
          _pos.set_size(_n_cities, 2);
          if (_is_round) {
               for(int i=0; i<_n_cities; ++i) {
                    double theta = 2 * numbers::pi * rnd.Rannyu();
                    _pos(i, 0) = side * cos(theta);
                    _pos(i, 1) = side * sin(theta);
               }
          } else {
               _pos = side*randu<mat>(_n_cities, 2);
          }
          _distances.set_size(_n_cities, _n_cities);
          for (int i = 0; i < _n_cities; ++i) {
               for (int j = i + 1; j < _n_cities; ++j) { // Parti da i+1
                    double d = norm(_pos.row(i) - _pos.row(j), 2);
                    _distances(i, j) = d;
                    _distances(j, i) = d; 
               }
          }         

     }
     void read_map_file(string filename) {
          ifstream in;
          in.open(filename);
          if (!in) {
               cerr << "Error opening file for reading map!" << endl;
               return;
          }
          _pos.set_size(_n_cities, 2);
          
          int dummy_index;
          for (int i = 0; i < _n_cities; ++i) {
               in >> dummy_index >> _pos(i, 0) >> _pos(i, 1);
          }
          in.close();
          _distances.zeros(_n_cities, _n_cities);
          for (int i = 0; i < _n_cities; ++i) {
               for (int j = i + 1; j < _n_cities; ++j) {
                    double d = norm(_pos.row(i) - _pos.row(j), 2);
                    _distances(i, j) = d;
                    _distances(j, i) = d;
               }
          }
     }
     double get_distance(int city_A, int city_B) const {
          return _distances(city_A, city_B);
     }
     void save_map() const {
          string filename = _is_round ? "round_map.dat" : "square_map.dat";
          ofstream out(filename);
          if (!out) {
               cerr << "Error opening file for writing map!" << endl;
               return;
          }
          for (int i = 0; i < _n_cities; ++i) {
               out << i << "\t" << _pos(i, 0) << "\t" << _pos(i, 1) << endl; 
          }
          out.close();
     }
     double get_x(int city_index) const {
          return _pos(city_index, 0);
     }
     double get_y(int city_index) const {
          return _pos(city_index, 1);
     }
     int get_n_cities() const {
          return _n_cities;
     }
};
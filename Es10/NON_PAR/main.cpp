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
#include <string>
#include <vector>
#include <iomanip>
#include <numbers>
#include <filesystem>
#include "../../PRNG/random.h"
#include "Population.h"
#include "Map.h"
#include "Individual.h"

using namespace std;

void print_progress_bar(int current_step, int total_steps) {
    int bar_width = 50; 
    float progress = (float)current_step / total_steps;
    
    int pos = bar_width * progress;
    cout << "[";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) {
            cout << "=";
        } else if (i == pos) {
            cout << ">";
        } else {
            cout << " ";
        }
    }
    cout << "] " << int(progress * 100.0) << " %\r";
    cout.flush();
    if (current_step == total_steps) {
        cout << endl;
    }
}

int main (int argc, char *argv[]){

    Random rnd;
    rnd.Initialize();

    GeneticAlg::load_input();

    filesystem::create_directories("OUTPUT");

    Map italy_map(false);
    italy_map.read_map_file("cap_prov_ita.dat");
    Population my_pop_italy(italy_map);
    my_pop_italy.initialize(rnd);   
    
    ofstream italy_out;
    italy_out.open("OUTPUT/italy_output.dat");
    
    for (int gen{}; gen < my_pop_italy.get_n_gens(); gen++) {
        
        my_pop_italy.sort_individuals();

        if (gen % 10000 == 0) {
            double best_path = my_pop_italy.get_best_fitness();
            double half_mean = my_pop_italy.compute_half_mean();
            italy_out << gen << "\t" << best_path << "\t" << half_mean << endl;
        }
        
        my_pop_italy.new_gen(rnd);
        my_pop_italy.mutate_generation(rnd);
        print_progress_bar(gen + 1, my_pop_italy.get_n_gens());
    }
    
    my_pop_italy.sort_individuals();
    Individual best_italy = my_pop_italy.get_best_individual(); 
    
    ofstream italy_path_out;
    italy_path_out.open("OUTPUT/italy_path.dat");
    if (italy_path_out.is_open()) {
        for (int i{}; i < italy_map.get_n_cities(); i++) {
            int city_index = best_italy.get_gene(i);
            double x = italy_map.get_x(city_index);
            double y = italy_map.get_y(city_index);
            italy_path_out << x << "\t" << y << endl;
        }
        italy_path_out.close();
    } else {
        cerr << "Errore: Impossibile creare il file italy_path.dat!" << endl;
    }

    rnd.SaveSeed();
    return 0;
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
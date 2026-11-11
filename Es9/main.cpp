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
#include "../PRNG/random.h"
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

    Map circ_map(true);
    circ_map.read_map_file("round_map.dat");
    //circ_map.generate_map(1, rnd);
    //circ_map.save_map();

    Population my_pop_circ(circ_map);
    my_pop_circ.initialize(rnd);

    ofstream circ_out;
    ofstream circ_history("OUTPUT/circ_history.dat");
    circ_out.open("OUTPUT/circ_output.dat");
    for (int gen{}; gen < my_pop_circ.get_n_gens(); gen++) {
        my_pop_circ.sort_individuals();
        double best_path =my_pop_circ.get_best_fitness();
        double half_mean = my_pop_circ.compute_half_mean();
        circ_out << gen << "\t" << best_path << "\t" << half_mean << endl;
        if (gen % 5 == 0 || gen == my_pop_circ.get_n_gens() - 1) {
            Individual current_best = my_pop_circ.get_best_individual();
            for (int i{}; i < circ_map.get_n_cities(); i++) {
                int city_index = current_best.get_gene(i);
                circ_history << gen << "\t" 
                             << circ_map.get_x(city_index) << "\t" 
                             << circ_map.get_y(city_index) << "\n";
            }
            int first_city = current_best.get_gene(0);
            circ_history << gen << "\t" 
                         << circ_map.get_x(first_city) << "\t" 
                         << circ_map.get_y(first_city) << "\n";
        }
        my_pop_circ.new_gen(rnd);
        my_pop_circ.mutate_generation(rnd);
        print_progress_bar(gen + 1, my_pop_circ.get_n_gens());
    }
    circ_history.close();
    my_pop_circ.sort_individuals();
    Individual best_circ = my_pop_circ.get_best_individual();

    ofstream path_out;
    path_out.open("OUTPUT/circ_path.dat");
    if (path_out.is_open()) {
        for (int i{}; i < circ_map.get_n_cities(); i++) {
            int city_index = best_circ.get_gene(i);
            double x = circ_map.get_x(city_index);
            double y = circ_map.get_y(city_index);
            path_out << x << "\t" << y << endl;
        }
        path_out.close();
    } else {
        cerr << "Errore: Impossibile creare il file circ_path.dat!" << endl;
    }
    
    circ_out.close();

    Map square_map(false);
    //square_map.generate_map(1, rnd);
    //square_map.save_map();
    square_map.read_map_file("square_map.dat");

    Population my_pop_square(square_map);
    my_pop_square.initialize(rnd);

    ofstream square_out;
    square_out.open("OUTPUT/square_output.dat");
    ofstream square_history("OUTPUT/square_history.dat");

    for (int gen{}; gen < my_pop_square.get_n_gens(); gen++) {
        my_pop_square.sort_individuals();
        double best_path = my_pop_square.get_best_fitness();
        double half_mean = my_pop_square.compute_half_mean();
        square_out << gen << "\t" << best_path << "\t" << half_mean << endl;
        if (gen % 5 == 0 || gen == my_pop_square.get_n_gens() - 1) {
            Individual current_best = my_pop_square.get_best_individual();
            for (int i{}; i < square_map.get_n_cities(); i++) {
                int city_index = current_best.get_gene(i);
                square_history << gen << "\t" 
                               << square_map.get_x(city_index) << "\t" 
                               << square_map.get_y(city_index) << "\n";
            }
            int first_city = current_best.get_gene(0);
            square_history << gen << "\t" 
                           << square_map.get_x(first_city) << "\t" 
                           << square_map.get_y(first_city) << "\n";
        }
        my_pop_square.new_gen(rnd);
        my_pop_square.mutate_generation(rnd);
        print_progress_bar(gen + 1, my_pop_square.get_n_gens());
    }

    square_history.close();
    square_out.close();
    my_pop_square.sort_individuals();
    Individual best_square = my_pop_square.get_best_individual();

    ofstream square_path_out;
    square_path_out.open("OUTPUT/square_path.dat");
    if (square_path_out.is_open()) {
        for (int i{}; i < square_map.get_n_cities(); i++) {
            int city_index = best_square.get_gene(i);
            double x = square_map.get_x(city_index);
            double y = square_map.get_y(city_index);
            square_path_out << x << "\t" << y << endl;
        }
        square_path_out.close();
    } else {
        cerr << "Errore: Impossibile creare il file square_path.dat!" << endl;
    }

    cout << "best L_circ: " << best_circ.get_fitness() << endl;
    cout << "best L_square: " << best_square.get_fitness() << endl;
    /*
    Map italy_map(false);
    italy_map.read_map_file("cap_prov_ita.dat");
    Population my_pop_italy(italy_map);
    my_pop_italy.initialize(rnd);   
    ofstream italy_out;
    italy_out.open("italy_output.dat");
    for (int gen{}; gen < my_pop_italy.get_n_gens(); gen++) {
        my_pop_italy.sort_individuals();
        double best_path = my_pop_italy.get_best_fitness();
        double half_mean = my_pop_italy.compute_half_mean();
        italy_out << gen << "\t" << best_path << "\t" << half_mean << endl;
        my_pop_italy.new_gen(rnd);
        my_pop_italy.mutate_generation(rnd);
        print_progress_bar(gen + 1, my_pop_italy.get_n_gens());
    }
    my_pop_italy.sort_individuals();
    Individual best_italy = my_pop_italy.get_best_individual(); 
    ofstream italy_path_out;
    italy_path_out.open("italy_path.dat");
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
   */
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

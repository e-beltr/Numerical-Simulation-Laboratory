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
#include <iomanip>
#include <cmath>
//#include <numbers>
#include <algorithm>
#include "mpi.h"
#include "../PRNG/random.h"
#include "Map.h"
#include "Individual.h"

using namespace std;

double compute_T(double T_min, double T_max, int N_node, int rank) {
    double T=T_min*pow(T_max/T_min, double(rank)/(N_node-1));
    return T;
}

int main (int argc, char *argv[]) {

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Random rnd;
    rnd.Initialize(rank);
    int config[3];
    double config_temp[2]; 

    if (rank == 0) {
        GeneticAlg::load_input();
        config[0] = GeneticAlg::get_n_cities();
        config[1] = GeneticAlg::get_n_steps();
        config[2] = GeneticAlg::get_n_migr();
        config_temp[0] = GeneticAlg::get_t_min();
        config_temp[1] = GeneticAlg::get_t_max();
    }
    MPI_Bcast(config, 3, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(config_temp, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    GeneticAlg::set_inputs(config[0], config[1], config[2]);
    GeneticAlg::set_temperature(config_temp[0], config_temp[1]);

    Map italy(1);   //da cambiare perche deve leggere da file
    italy.read_map_file("cap_prov_ita.dat");
    int n_cities = italy.get_n_cities();
    
    Individual old_ind;
    old_ind.compute_fitness(italy);
    double L_old = old_ind.get_fitness();
    double L_new = 0.0;
    
    int steps = GeneticAlg::get_n_steps();
    double T_min = GeneticAlg::get_t_min();
    double T_max = GeneticAlg::get_t_max();
    double T = compute_T(T_min, T_max, size, rank);
    int partner = -1;

    int save_freq_L = 10000;
    long int save_step = 1;
    long int next_save = 1;

    ofstream out_L;
    ofstream out_history;

    if (rank == 0) {
        out_L.open("OUTPUT/L_rank0.dat");
	out_history.open("OUTPUT/history_path.dat");
        if (out_L.is_open()) {
            out_L << "0\t" << L_old << endl;
        }
    if (out_history.is_open()) {
            for (int j = 0; j < n_cities; j++) {
                int city_index = old_ind.get_gene(j);
                out_history << "0\t" 
                            << italy.get_x(city_index) << "\t" 
                            << italy.get_y(city_index) << endl;
            }
            // Chiusura del circuito
            out_history << "0\t" 
                        << italy.get_x(old_ind.get_gene(0)) << "\t" 
                        << italy.get_y(old_ind.get_gene(0)) << endl;
        }
    }

    for (int i = 1; i <= steps; i++) {
        Individual new_ind;
        do {
            new_ind = old_ind;
            new_ind.mutate(rnd);
        } while (!new_ind.check());
        new_ind.compute_fitness(italy);
        L_new = new_ind.get_fitness();
        double delta_L = L_new - L_old;
        if (delta_L < 0 || rnd.Rannyu() < exp(-delta_L / T)) {
            old_ind = new_ind;
            L_old = L_new;
        }

        if (i % GeneticAlg::get_n_migr() == 0) {
            int turn = i/1000;
            if (turn%2 == 0) {
                if (rank%2 == 0) partner = rank + 1;
                else partner = rank - 1;
            } else {
                if (rank%2 == 0) partner = rank - 1;
                else partner = rank + 1;
            }
            if (partner >= size || partner < 0) partner = -1;
            
            if (partner != -1) {
                double my_L = L_old;
                double my_beta = 1/T;
                double my_data[2] = {my_L, my_beta};
                double L_partner, beta_partner;
                double partner_data[2] = {L_partner, beta_partner};
                int is_accepted = 0;
                if (rank > partner) {
                    MPI_Send(my_data, 2, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD);
                    MPI_Recv(&is_accepted, 1, MPI_INT, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                } else {
                    MPI_Recv(partner_data, 2, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    double delta[2] = {partner_data[0] - my_data[0], my_data[1] - partner_data[1]}; 
                    double product = delta[0]*delta[1];
                    if (product < 0.0 || rnd.Rannyu() < exp(-product)) is_accepted = 1; 
                    MPI_Send(&is_accepted, 1, MPI_INT, partner, 1, MPI_COMM_WORLD);
                }
                if (is_accepted == 1) {
                    int old_path[n_cities];
                    int new_path[n_cities];
                    for (int j{}; j < n_cities; j++) old_path[j] = old_ind.get_gene(j);
                    MPI_Sendrecv(
                        old_path, n_cities, MPI_INT, partner, 2, 
                        new_path, n_cities, MPI_INT, partner, 2,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE
                    );
                    for (int j{}; j < n_cities; j++) old_ind.set_gene(j, new_path[j]); 
                    old_ind.compute_fitness(italy);
                    L_old = old_ind.get_fitness();                       
                }
            }
        }
        if (rank == 0 && (i% save_freq_L==0 || i == steps)) {
            out_L << i << "\t" << L_old << endl;
        }
	if (rank == 0 && (i == next_save || i == steps)) {
            if (out_history.is_open()) {
                for (int j = 0; j < n_cities; j++) {
                    int city_index = old_ind.get_gene(j);
                    out_history << i << "\t"
                                << italy.get_x(city_index) << "\t"
                                << italy.get_y(city_index) << endl;
                }
                // Chiusura del circuito
                out_history << i << "\t"
                            << italy.get_x(old_ind.get_gene(0)) << "\t"
                            << italy.get_y(old_ind.get_gene(0)) << endl;
            }
            if (next_save == save_step * 10) {
                save_step *= 10;
            }
            next_save += save_step;
        }
        
    }


    if (rank == 0) {
	if (out_L.is_open()) out_L.close();
	if (out_history.is_open()) out_history.close();

        Individual best_ind = old_ind;
        best_ind.compute_fitness(italy);
        double L_best = best_ind.get_fitness();
        
        
        ofstream out("OUTPUT/best_path.dat");
        if (out.is_open()) {
            for (int i{}; i < italy.get_n_cities(); i++) {
                int city_index = best_ind.get_gene(i);
                double x = italy.get_x(city_index);
                double y = italy.get_y(city_index);
                out << x << "\t" << y << endl;
            }
            out.close();
        } else {
            cerr << "Errore: Impossibile creare il file best_path.dat!" << endl;
        }
        cout << "Best path length: " << L_best << endl;
    }

    MPI_Finalize();
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

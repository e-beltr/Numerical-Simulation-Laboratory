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
#include <numbers>
#include <iomanip>
#include <cmath>
#include "../../PRNG/random.h"
#include "Metro.h"

using namespace std;

void print_progress(int current_step, int total_steps, double current_T) {
    // Calcola la percentuale
    int progress = (int)((current_step / (double)total_steps) * 100.0);
    
    // Costruisci la barra
    int barWidth = 50;
    int pos = (progress * barWidth) / 100;
    
    cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "#";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    // Stampa la barra sovrascrivendo la riga precedente (\r)
    cout << "] " << progress << "% (T = " << fixed << setprecision(5) << current_T << ")" << flush;
}
 
int main (int argc, char *argv[]){

    Random rnd;
    rnd.Initialize();

    int M{100000};              // Total number of throws for the Monte Carlo simulation
    int N{100};                 // Number of blocks for Data Blocking
    double T{1.0};              // Temperature for the Metropolis algorithm (SA)
    double T_min{0.0001};       // Minimum temperature for the simulated annealing
    double cooling_rate{0.9};   // Cooling rate for the simulated annealing
    double par_step{0.1};          // Step size for parameter adjustments in the annealing process

    Metro my_metro(M, N, rnd, 1.2, 1, 1);               // Create Metro object with a step size of 1.2, mu = 1 and sigma = 1
    my_metro.auto_set_delta();                          // Automatically adjust the step size to achieve an acceptance rate of ~50%
    my_metro.run_DB();                                  // Run the Data Blocking simulation 
    double old_ene = my_metro.get_final_ave();          // Get the final average energy from the first run
    double error = my_metro.get_final_err();            // Get the final error from the first run
    double new_ene = 0;                                 // Initialize new energy variable for the annealing loop
    double new_error = 0;                               // Initialize new error variable for the annealing loop
    double old_mu = my_metro.get_mu();                   // Store the initial μ parameter   
    double old_sigma = my_metro.get_sigma();             // Store the initial σ parameter
    double new_mu = 0;
    double new_sigma = 0;
    double internal_acc = 0;                             // Variable to track the acceptance rate during annealing
    ofstream out("OUTPUT/annealing_path.dat");                   // Open a file to save the annealing path (μ, σ, energy)
    if (out.is_open()) {
        out << fixed << setprecision(10); // Set fixed decimal precision for clean output files
        out << "# T\t\tmu\t\t sigma\t\t energy\t\t error\t\tinternal_acc\t\n"; // Header for the output file
    } else {
        cerr << "ERROR: Impossible to open the output file" << endl;
        return 1; // Exit with error code if file cannot be opened
    }

    int total_temp_steps = ceil(log(T_min / T) / log(cooling_rate));
    int current_step = 0;
    cout << "Inizio Simulated Annealing. Totale step previsti: " << total_temp_steps << endl;

    out << T << "\t" << old_mu << "\t" << old_sigma << "\t" << old_ene << "\t" << error << "\t" << internal_acc << endl; // Save the initial state before annealing

    while (T > T_min) {    
        double current_par_step = par_step * sqrt(T);                         // Simulated annealing loop
        for (int i{}; i<100; i++) {    // Perform a number of Metropolis steps at the current temperature
            new_mu = abs(old_mu + current_par_step*rnd.Rannyu(-1.,1.));           // Propose a new μ by adding a random perturbation to the old μ
            new_sigma = abs(old_sigma + current_par_step*rnd.Rannyu(-1.,1.));     // Propose a new σ by adding a random perturbation to the old σ
            my_metro.set_parameters(new_mu, new_sigma);                   // Adjust the parameters of the trial wavefunction slightly to explore the parameter space
            //my_metro.equilibrate(100);                               // Equilibrate the system at the new parameters before measuring
            my_metro.reset_acc();                                         // Reset the internal state of the Metro object for a fresh simulation
            my_metro.run_DB();                                            // Run another Data Blocking simulation at the new temperature
            new_ene = my_metro.get_final_ave();                           // Get the final average energy from the current run
            new_error = my_metro.get_final_err();                         // Get the final error from the current run
            double delta_ene = new_ene - old_ene;                         // Calculate the change in energy
            if (delta_ene < 0 || rnd.Rannyu() < exp(-delta_ene / T)) { // Accept the new state if it's better or with a probability based on the energy difference and current temperature
                old_ene = new_ene;                              // Update the old energy to the new energy
                old_mu = new_mu;                     // Update the old mu to the new mu
                old_sigma = new_sigma;               // Update the old sigma to the new sigma
                error = new_error;                   // Update the error to the new error
            }
        }
        internal_acc = my_metro.get_acc_rate(); // Get the acceptance rate at the current temperature
        out << T << "\t" << old_mu << "\t" << old_sigma << "\t" << old_ene << "\t" << error << "\t" << internal_acc << endl; // Save the current state after each temperature step
        current_step++;
        print_progress(current_step, total_temp_steps, T);
        T *= cooling_rate;                             // Cool down the system
    }
    double best_mu = old_mu; // Store the best μ found during annealing
    double best_sigma = old_sigma; // Store the best σ found during annealing
    out.close();

    Metro best_metro(M*10, N*10, rnd, 1.2, best_mu, best_sigma);      // Create a new Metro object with the best parameters found
    best_metro.auto_set_delta();                                // Automatically adjust the step size for the best parameters
    best_metro.set_start_position(best_mu);                   // Start the new simulation at the best μ found (which is around the maximum of the probability distribution)
    best_metro.equilibrate(1000);                               // Equilibrate the system
    best_metro.run_DB();                                        // Run a final Data Blocking simulation with the best parameters
    best_metro.save_dat("best_parameters.dat", 0.0);            // Save the results of the final simulation to a file
    best_metro.sample_positions(1000000, "positions.dat");
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
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
#include <numbers>
#include "../../PRNG/random.h"
#include "DataBlocking.h"
#include "RandomWalk.h"

using namespace std;

int main (int argc, char *argv[]){

    Random rnd;                                              // Instantiates the pseudo-random number generator
    rnd.Initialize();                                        // Initializes RNG reading seed and primes

    int M{100000};                                           // Total number of simulated random walks
    int N{100};                                              // Number of statistical blocks
    int steps{100};                                          // Number of discrete time steps per walk

    // 1. Discrete Random Walk
    discrete_RW discr(M, N, steps, rnd);                     // Instantiates discrete RW with data blocking
    discr.run_DB();                                          // Executes simulation and block analysis
    discr.save_dat("discrRW.dat");                           // Saves progressive average and error to file

    // 2. Continuous Random Walk
    continuous_RW cont(M, N, steps, rnd);                    // Instantiates continuous RW with data blocking
    cont.run_DB();                                           // Executes simulation and block analysis
    cont.save_dat("contRW.dat");                             // Saves progressive average and error to file

    // 3. Single RW trajectories for 3D visualization
    double pos_discr[3] = {0.0, 0.0, 0.0};                   // Starting coordinates for single discrete walk
    double pos_cont[3]  = {0.0, 0.0, 0.0};                   // Starting coordinates for single continuous walk
    ofstream out_d("OUTPUT/RW_discr_path.dat");                   // Output file stream for discrete 3D trajectory
    ofstream out_c("OUTPUT/RW_cont_path.dat");                    // Output file stream for continuous 3D trajectory

    if (out_d.is_open() && out_c.is_open()) {                // Checks that both trajectory files are open
        out_d << pos_discr[0] << "\t" << pos_discr[1] << "\t" << pos_discr[2] << endl; // Prints origin for discrete walk
        out_c << pos_cont[0]  << "\t" << pos_cont[1]  << "\t" << pos_cont[2]  << endl; // Prints origin for continuous walk

        for (int k{}; k < steps; k++) {                      // Generates individual steps for visualization
            // Step discreto
            int dir = int(rnd.Rannyu(0.0, 3.0));             // Chooses random Cartesian axis (0, 1, 2)
            double verso = (rnd.Rannyu() < 0.5) ? 1.0 : -1.0; // Chooses direction forward or backward
            pos_discr[dir] += verso;                         // Updates discrete coordinate
            out_d << pos_discr[0] << "\t" << pos_discr[1] << "\t" << pos_discr[2] << endl; // Writes discrete position

            // Step continuo
            double phi{rnd.Rannyu(0, 2 * numbers::pi)};      // Uniform azimuthal angle in [0, 2*pi)
            double cos_theta{1 - 2 * rnd.Rannyu()};          // Uniform cos(theta) in [-1, 1] for isotropy
            double sin_theta{sqrt(1 - cos_theta * cos_theta)}; // Calculates sin(theta) from trigonometric identity
            pos_cont[0] += sin_theta * cos(phi);             // Projects step along x axis
            pos_cont[1] += sin_theta * sin(phi);             // Projects step along y axis
            pos_cont[2] += cos_theta;                        // Projects step along z axis
            out_c << pos_cont[0] << "\t" << pos_cont[1] << "\t" << pos_cont[2] << endl; // Writes continuous position
        }
        out_d.close();                                       // Closes discrete trajectory stream
        out_c.close();                                       // Closes continuous trajectory stream
    } else {
        cerr << "ERRORE: Impossibile creare i file per le traiettorie 3D in OUTPUT!" << endl; // Error handling
    }

    rnd.SaveSeed();                                          // Saves current RNG state to file
    return 0;                                                // Terminates program execution
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

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
#include <cstdlib>
#include "system.h"

using namespace std;

void ask_confirm () {
  char answer;
  cout << "Sei sicurissimo di voler far partire la simulazione? Hai messo i dati giusti per la simulazione??  (y/n) ";
  cin >> answer;
  while (answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N') {
      cout << "Carattere non valido. Inserisci 'y' (si) o 'n' (no): ";
      cin >> answer;
  }
  if (answer == 'n' || answer == 'N') {
      cout << "Simulation blocked. Exiting..." << endl;
      exit(0);
  }
}

int main (int argc, char *argv[]){

  System SYS;
  SYS.initialize();
  SYS.initialize_properties();
  SYS.block_reset(0);
  ask_confirm();
  SYS.equilibration(2000);
  int barWidth = 50;

  for(int i=0; i < SYS.get_nbl(); i++){ //loop over blocks
    for(int j=0; j < SYS.get_nsteps(); j++){ //loop over steps in a block
      SYS.step();
      SYS.measure();
//      if(j%50 == 0){
//        SYS.write_XYZ(nconf); //Write actual configuration in XYZ format //Commented to avoid "filesystem full"! 
//        nconf++;
//     }
    float progress = (float)(i + 1) / SYS.get_nbl();
    cout << "[";
    int pos = barWidth * progress;
    for (int k = 0; k < barWidth; ++k) {
        if (k < pos) cout << "=";
        else if (k == pos) cout << ">";
        else cout << " ";
    }
    // '\r' riporta il cursore all'inizio della riga per sovrascriverla al prossimo giro
    cout << "] " << int(progress * 100.0) << " %\r";
    cout.flush(); // Forza la stampa sul terminale senza aspettare l'endl
    }
    SYS.averages(i+1);
    SYS.block_reset(i+1);
  }
  SYS.finalize();
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

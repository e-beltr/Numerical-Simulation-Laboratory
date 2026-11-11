#!/bin/bash

T_START=2.0
T_END=0.5
T_STEP=0.1
INPUT_FILE="../INPUT/input.dat"
OUTPUT_DIR="../OUTPUT"
CONFIG_IN="../INPUT/CONFIG/config.spin"
CONFIG_OUT="../OUTPUT/CONFIG/config.spin"
EXECUTABLE="./simulator.exe"

# ==============================================================================
# FASE 1: SIMULAZIONE A h = 0.0 (U, C, chi)
# ==============================================================================
echo "=== AVVIO CICLO h = 0.0 (Energy, Specific Heat, Susceptibility) ==="

# Pulizia mirata dei vecchi dati a h=0
rm -f $OUTPUT_DIR/summary_energy.dat
rm -f $OUTPUT_DIR/summary_cv.dat
rm -f $OUTPUT_DIR/summary_chi.dat

# Imposta h = 0.0 nel file di input
sed -i '' "s/^SIMULATION_TYPE.*/SIMULATION_TYPE 2 1.0 0.0/" $INPUT_FILE 2>/dev/null || sed -i "s/^SIMULATION_TYPE.*/SIMULATION_TYPE 2 1.0 0.0/" $INPUT_FILE
# Se il tuo input.dat ha invece una riga dedicata ad H (es. "H 0.0"):
# sed -i '' "s/^H .*/H 0.0/" $INPUT_FILE 2>/dev/null || sed -i "s/^H .*/H 0.0/" $INPUT_FILE

RESTART_VAL=0

for T in $(seq -f "%.1f" $T_START -$T_STEP $T_END); do
    echo "--- h = 0.0 | T = $T ---"
    sed -i '' "s/^TEMP.*/TEMP $T/" $INPUT_FILE 2>/dev/null || sed -i "s/^TEMP.*/TEMP $T/" $INPUT_FILE
    sed -i '' "s/^RESTART.*/RESTART $RESTART_VAL/" $INPUT_FILE 2>/dev/null || sed -i "s/^RESTART.*/RESTART $RESTART_VAL/" $INPUT_FILE
    
    $EXECUTABLE

    tail -n 1 $OUTPUT_DIR/total_energy.dat  | awk -v t=$T '{print t, $3, $4}' >> $OUTPUT_DIR/summary_energy.dat
    tail -n 1 $OUTPUT_DIR/specific_heat.dat | awk -v t=$T '{print t, $3, $4}' >> $OUTPUT_DIR/summary_cv.dat
    tail -n 1 $OUTPUT_DIR/susceptibility.dat | awk -v t=$T '{print t, $3, $4}' >> $OUTPUT_DIR/summary_chi.dat

    RESTART_VAL=1
    cp $CONFIG_OUT $CONFIG_IN
done


# ==============================================================================
# FASE 2: SIMULAZIONE A h = 0.02 (M)
# ==============================================================================
echo "=== AVVIO CICLO h = 0.02 (Magnetization) ==="

# Pulizia mirata del vecchio file di magnetizzazione
rm -f $OUTPUT_DIR/summary_mag.dat

# Imposta h = 0.02 nel file di input
sed -i '' "s/^SIMULATION_TYPE.*/SIMULATION_TYPE 2 1.0 0.02/" $INPUT_FILE 2>/dev/null || sed -i "s/^SIMULATION_TYPE.*/SIMULATION_TYPE 2 1.0 0.02/" $INPUT_FILE
# Se hai la riga dedicata:
# sed -i '' "s/^H .*/H 0.02/" $INPUT_FILE 2>/dev/null || sed -i "s/^H .*/H 0.02/" $INPUT_FILE

# Si riparte da 0 per termalizzare con il campo magnetico acceso da T=2.0
RESTART_VAL=0

for T in $(seq -f "%.1f" $T_START -$T_STEP $T_END); do
    echo "--- h = 0.02 | T = $T ---"
    sed -i '' "s/^TEMP.*/TEMP $T/" $INPUT_FILE 2>/dev/null || sed -i "s/^TEMP.*/TEMP $T/" $INPUT_FILE
    sed -i '' "s/^RESTART.*/RESTART $RESTART_VAL/" $INPUT_FILE 2>/dev/null || sed -i "s/^RESTART.*/RESTART $RESTART_VAL/" $INPUT_FILE
    
    $EXECUTABLE

    tail -n 1 $OUTPUT_DIR/magnetization.dat | awk -v t=$T '{print t, $3, $4}' >> $OUTPUT_DIR/summary_mag.dat

    RESTART_VAL=1
    cp $CONFIG_OUT $CONFIG_IN
done

echo "Tutte le simulazioni sono state completate con successo."
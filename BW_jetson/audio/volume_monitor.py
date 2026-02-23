#!/usr/bin/env python3
import subprocess
import numpy as np
import time
import os

# Parametri microfono e registrazione
MIC_DEVICE = "hw:2,0"
SAMPLE_RATE = 48000
CHANNELS = 1
CHUNK_DURATION = 0.1  # secondi per finestra RMS
THRESHOLD = 2500      # soglia RMS da calibrare
AMPLIFY_FACTOR = 1  # opzionale, per segnale molto basso
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
FLAG_FILE = os.path.join(SCRIPT_DIR, "audio_detection.txt")  # file da aggiornare

# Numero di campioni per chunk
num_samples = int(SAMPLE_RATE * CHUNK_DURATION)

# comando arecord in streaming (senza -d)
cmd = [
    "arecord",
    "-D", MIC_DEVICE,
    "-f", "S16_LE",
    "-c", str(CHANNELS),
    "-r", str(SAMPLE_RATE),
    "-t", "raw"
]

#print("Rilevamento rumore in corso... premi Ctrl+C per fermare")

try:
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    while True:
        raw = proc.stdout.read(num_samples * 2)  # 2 byte per int16
        if len(raw) < num_samples * 2:
            continue  # lettura incompleta
        chunk = np.frombuffer(raw, dtype=np.int16)

        # opzionale: amplifica
        chunk_float = chunk.astype(np.float32) * AMPLIFY_FACTOR

        # calcola RMS
        rms = np.sqrt(np.mean(chunk_float**2))

        # scrivi "noise" o spazio vuoto sul file
        #print(f"\nRMS={rms:.1f}", end="")
        with open(FLAG_FILE, "w") as f:
            if rms > THRESHOLD:
                f.write("noise\n")
                #print("  <<< RUMORE RILEVATO!")
            else:
                f.write(" ")

except KeyboardInterrupt:
    proc.terminate()
    #print("\nRilevamento interrotto dall'utente.")


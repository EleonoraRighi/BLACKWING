#!/bin/bash

set -e

BASE_DIR="/home/airlab/BW_jetson"
PIDS=()

cleanup() {
    echo ""
    echo "🛑 KeyInterrupt detected. Shutting down all processes..."
    for pid in "${PIDS[@]}"; do
        kill "$pid" 2>/dev/null || true
    done
    exit 0
}

trap cleanup SIGINT SIGTERM

echo "=== STARTING BW_jetson SYSTEM ==="

# -------------------------
# Audio
# -------------------------
echo "Starting volume_monitor.py..."
python3 "$BASE_DIR/audio/volume_monitor.py" &
PIDS+=($!)

# -------------------------
# Vision
# -------------------------
echo "Starting video_monitor.py..."
python3 "$BASE_DIR/vision/video_monitor.py" &
PIDS+=($!)

# -------------------------
# Speaker
# -------------------------
echo "Starting speaker_server.py..."
python3 "$BASE_DIR/speaker/speaker_server.py" &
PIDS+=($!)


# -------------------------
# Build + esecuzione core
# -------------------------
echo "Building..."
cd "$BASE_DIR/build"

cmake ..
make -j"$(nproc)"

echo "Startting 'start_bw'..."
./start_bw &
PIDS+=($!)

echo "=== SYSTEM STARTED. Ctrl+C to stop ==="

wait

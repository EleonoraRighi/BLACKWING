import socket
import os
import threading
import subprocess

SOCKET_PATH = "/tmp/audio_socket"
AUDIO_FOLDER = os.path.join(os.path.dirname(__file__), "tracks")

class AudioServer:
    def __init__(self):
        # Lista dei file WAV disponibili
        self.sounds = [f for f in os.listdir(AUDIO_FOLDER) if f.endswith(".wav")]
        print(f"{len(self.sounds)} files loaded")

        if os.path.exists(SOCKET_PATH):
            os.remove(SOCKET_PATH)
        self.server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.server.bind(SOCKET_PATH)
        self.server.listen(5)

    def _play_sound_thread(self, filename):
        path = os.path.join(AUDIO_FOLDER, filename)
        # Usa aplay con plughw per compatibilità totale con USB speaker
        subprocess.run(["aplay", "-q", "-D", "plughw:2,0", path])

    def run(self):
        while True:
            conn, _ = self.server.accept()
            data = conn.recv(1024).decode().strip()
            conn.close()

            if data in self.sounds:
                threading.Thread(target=self._play_sound_thread, args=(data,), daemon=True).start()
            else:
                print(f"Record not found: {data}")

if __name__ == "__main__":
    server = AudioServer()
    server.run()

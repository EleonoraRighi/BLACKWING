#include "tracks_manager.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define SOCKET_PATH "/tmp/audio_socket"

void play_sound(const char *filename)
{
    int sock;
    struct sockaddr_un addr;

    // Crea socket UNIX
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    // Connessione al server Python
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        // Invia il nome del file WAV
        if (write(sock, filename, strlen(filename)) < 0) {
            perror("write failed");
        } else {
            //printf("Requested playback: %s\n", filename);
        }
    } else {
        perror("connect to audio socket failed");
    }

    close(sock);
}

#include "main.h"
#include <ncurses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXPENDING 5

int main() {
    // Inizializza ncurses
    initscr();
    noecho();
    curs_set(0);
    
    // Setup del socket
    int serverPort = 12345;
    struct sockaddr_in serverAddr;
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) {
        endwin();
        perror("socket() failed");
        return 1;
    }

    int optval = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        endwin();
        perror("bind() failed");
        return 1;
    }

    if (listen(serverSocket, MAXPENDING) < 0) {
        endwin();
        perror("listen() failed");
        return 1;
    }

    // Loop principale per accettare connessioni
    for(;;) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        mvprintw(0, 0, "Waiting for connection...");
        refresh();
        
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            perror("accept() failed");
            continue; // Continua ad ascoltare per altre connessioni
        }

        mvprintw(0, 0, "Client connected!     ");
        refresh();
    
        // Loop di gestione del client corrente
        Personaggio guardia = {0};
        guardia.x = -1;

        while (1) {
            Personaggio buffer;
            int recvSize = recv(clientSocket, &buffer, sizeof(Personaggio), 0);
            
            if (recvSize <= 0) {
                break; // Client disconnesso o errore
            }

            if (recvSize != sizeof(Personaggio)) {
                continue; // Dati incompleti
            }

            // Cancella la vecchia posizione
            if (guardia.x != -1) {
                mvaddch(guardia.y, guardia.x, ' ');
            }

            // Disegna la nuova posizione
            mvaddch(buffer.y, buffer.x, buffer.ch);
            refresh();
            
            guardia = buffer;
        }

        // Pulisci lo schermo quando il client si disconnette
        clear();
        refresh();
        close(clientSocket);
    }

    // Cleanup finale
    endwin();
    close(serverSocket);
    return 0;
}
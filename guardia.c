#include "main.h"
#include <ncurses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    // Initialize ncurses
    initscr();
    noecho();             // Don't echo keystrokes
    cbreak();             // Disable line buffering
    keypad(stdscr, TRUE); // Enable keypad for arrow keys

    int clientSocket;
    struct sockaddr_in serverAddr;
    int serverPort = 12345;

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0)
    {
        endwin(); // Clean up ncurses before exiting
        perror("socket() failed");
        return 1;
    }

    

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        endwin(); // Clean up ncurses before exiting
        perror("connect() failed");
        return 1;
    }

    Personaggio guardia = {LINES - 1, COLS - 1, 'G'};
    int ch; // Changed from char to int for special keys

    while (1)
    {
        ch = getch(); // Get character or special key

        switch (ch)
        {
        case KEY_LEFT:
            if (guardia.x > 0)
                guardia.x--;
            break;
        case KEY_RIGHT:
            if (guardia.x < COLS - 1)
                guardia.x++;
            break;
        case KEY_UP:
            if (guardia.y > 0)
                guardia.y--;
            break;
        case KEY_DOWN:
            if (guardia.y < LINES - 1)
                guardia.y++;
            break;
        case 'q': // Add quit condition
        case 'Q':
            endwin();
            close(clientSocket);
            return 0;
        default:
            break;
        }

        int charlen = sizeof(guardia);
        int len = send(clientSocket, &guardia, sizeof(guardia), 0);
        if (len != charlen)
        {
            endwin(); // Clean up ncurses before exiting
            perror("send() failed");
            close(clientSocket);
            return 1;
        }
    }

    endwin();
    close(clientSocket);
    return 0;
}
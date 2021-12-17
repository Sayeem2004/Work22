#include "pipe_networking.h"

// Variable Declarations
char *buff;
int to_server, from_server;

void client_exit() {
    // Removing Secret Pipe
    char *name = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    sprintf(name, "%d", getpid());
    int err1 = remove(name);
    if (err1 == -1) {
        print_error(-1, "Client: Unable To Remove Secret Pipe");
    } else printf("Client: Successfully Removed Secret Pipe\n");

    // Closing Write File
    int err2 = close(to_server);
    if (err2 == -1) {
        print_error(-1, "Client: Unable To Close Write File");
    } else printf("Client: Successfully Closed Write File\n");

    // Closing Read File
    int err3 = close(from_server);
    if (err3 == -1) {
        print_error(-1, "Client: Unable To Close Read File");
    } else printf("Client: Successfully Closed Read File\n");

    // Exiting Function
    free(buff);
    printf("Client: Successfully Shut Down\n");
    exit(0);
}

static void sighandler(int signo) {
    // SIGINT Case
    if (signo == SIGINT) {
        // Exiting Normally
        client_exit();
    }

    // Exiting Function
    return;
}

int main() {
    // Signal Handling
    signal(SIGINT, sighandler);

    // Completing Handshake
    from_server = client_handshake(&to_server);

    // Initializing Buffer
    buff = calloc(BUFFER_SIZE, sizeof(char));

    // Constantly Runs
    while (1) {
        // Asking For User Input
        printf("Input String: ");
        fgets(buff, BUFFER_SIZE, stdin);
        *strchr(buff, '\n') = '\0';

        // Quit Or Exit Command
        if (strcmp(buff, "exit") == 0 || strcmp(buff, "quit") == 0) break;

        // Writing To User To Worker Pipe
        int err1 = write(to_server, buff, BUFFER_SIZE);
        if (err1 == -1) {
            print_error(-1, "Unable To Send Input To Server");
            continue;
        }

        // Reading From Worker To User Pipe
        int err2 = read(from_server, buff, BUFFER_SIZE);
        if (err2 == -1) {
            print_error(-1, "Unable To Read Output From Server");
            continue;
        }
        printf("Output String: %s\n", buff);
    }

    // Exiting Normally
    client_exit();
    return 0;
}

#include "pipe_networking.h"

void print_error(int err, char *msg) {
    if (err == -1) {
        // If Error Print Error Message and Errno
        printf("Error: %s\n", msg);
        printf("%s\n", strerror(errno));
    }
}

char *start_server_handshake(int *from_client) {
    // Making Well Known Pipe
    int err1 = mkfifo(WKP, 0644);
    if (err1 == -1) {
        print_error(-1, "Server: Unable To Make Well Known Pipe");
        return NULL;
    } else printf("Server: Successfully Made Well Known Pipe\n");

    // Opening Well Known Pipe
    int read_file = open(WKP, O_RDONLY);
    if (read_file == -1) {
        print_error(-1, "Server: Unable To Open Well Known Pipe");
        return NULL;
    } else printf("Server: Successfully Opened Well Known Pipe\n");

    // Receiving Message Through Well Known Pipe
    char *name = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    int err2 = read(read_file, name, HANDSHAKE_BUFFER_SIZE);
    if (err2 == -1) {
        print_error(-1, "Server: Unable To Read Secret Message");
        return NULL;
    } else printf("Server: Successfully Read Secret Message\n");

    // Exiting Function
    *from_client = read_file;
    return name;
}

void reset_server_handshake(int from_client) {
    // Removing Well Known Pipe
    int err1 = remove(WKP);
    if (err1 == -1) {
        print_error(-1, "Server: Unable To Remove Well Known Pipe");
    } else printf("Server: Successfully Removed Well Known Pipe\n");

    // Closing Read File
    int err3 = close(from_client);
    if (err3 == -1) {
        print_error(-1, "Server: Unable To Close Read File");
    } else printf("Server: Successfully Closed Read File\n");

    // Exiting Function
    return;
}

int finish_server_handshake(char *name, int read_file) {
    // Opening Secret Pipe
    int write_file = open(name, O_WRONLY);
    if (write_file == -1) {
        print_error(-1, "Sub-Server: Unable To Open Secret Pipe");
        return -1;
    } else printf("Sub-Server: Successfully Opened Secret Pipe\n");

    // Sending Verification Message
    int err4 = write(write_file, ACK, HANDSHAKE_BUFFER_SIZE);
    if (err4 == -1) {
        print_error(-1, "Sub-Server: Unable To Send Verification Message");
        return -1;
    } else printf("Sub-Server: Successfully Sent Verification Message\n");

    // Receiving Verification Message
    char *buff = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    int err5 = read(read_file, buff, HANDSHAKE_BUFFER_SIZE);
    if (err5 == -1) {
        print_error(-1, "Sub-Server: Unable To Receive Verification Message");
        return -1;
    } else printf("Sub-Server: Successfully Received Verification Message\n");

    // Checking Verification Message
    if (strcmp(buff, ACK) != 0) {
        print_error(-1, "Sub-Server: Unable To Match Verification Message");
        return -1;
    } else printf("Sub-Server: Successfully Matched Verification Message\n");

    // Exiting Function
    printf("Sub-Server: Successfully Completed 3-Way Handshake\n");
    return write_file;
}

int client_handshake(int *to_server) {
    // Creating Secret Pipe
    char *name = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    sprintf(name, "%d", getpid());
    int err1 = mkfifo(name, 0644);
    if (err1 == -1) {
        print_error(-1, "Client: Unable To Make Secret Pipe");
        return -1;
    } else printf("Client: Successfully Made Secret Pipe\n");

    // Opening Well Known Pipe
    int write_file = open(WKP, O_WRONLY);
    if (write_file == -1) {
        print_error(-1, "Client: Unable To Open Well Known Pipe");
        return -1;
    } else printf("Client: Successfully Opened Well Known Pipe\n");

    // Sending Message Through Well Known Pipe
    int err2 = write(write_file, name, HANDSHAKE_BUFFER_SIZE);
    if (err2 == -1) {
        print_error(-1, "Client: Unable To Send Secret Pipe");
        return -1;
    } else printf("Client: Successfully Sent Secret Pipe\n");

    // Opening Secret Pipe
    int read_file = open(name, O_RDONLY);
    if (read_file == -1) {
        print_error(-1, "Client: Unable To Open Secret Pipe");
        return -1;
    } else printf("Client: Successfully Opened Secret Pipe\n");

    // Removing Secret Pipe
    int err3 = remove(name);
    if (err3 == -1) {
        print_error(-1, "Client: Unable To Remove Secret Pipe");
        return -1;
    } else printf("Client: Successfully Removed Secret Pipe\n");

    // Receiving Verification Message
    char *buff = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    int err4 = read(read_file, buff, HANDSHAKE_BUFFER_SIZE);
    if (err4 == -1) {
        print_error(-1, "Client: Unable To Receive Verification Message");
        return -1;
    } else printf("Client: Successfully Received Verification Message\n");

    // Checking Verification Message
    if (strcmp(buff, ACK) != 0) {
        print_error(-1, "Client: Unable To Match Verification Message");
        return -1;
    } else printf("Client: Successfully Matched Verification Message\n");

    // Sending Verification Message
    int err5 = write(write_file, ACK, HANDSHAKE_BUFFER_SIZE);
    if (err5 == -1) {
        print_error(-1, "Client: Unable To Send Verification Message");
        return -1;
    } else printf("Client: Successfully Sent Verification Message\n");

    // Print that Process Is Finished
    printf("Client: Successfully Completed 3-Way Handshake\n");

    // Freeing Data
    free(name);
    free(buff);

    // Returning File Descriptors
    *to_server = write_file;
    int from_server = read_file;
    return from_server;
}

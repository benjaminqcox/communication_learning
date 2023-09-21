#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CONNECTION_ADDRESS "127.0.0.1"
#define SERVER_PORT 18000
#define BACKLOG_SIZE 10
#define TCP_MODE 0

int main()
{
    int ret, sd;
    struct sockaddr_in sv_addr; // Server structure
    socklen_t addrlen;
    ssize_t received_bytes, sent_bytes;
    time_t cl_timestamp, sv_timestamp;

    // Create socket
    sd = socket(AF_INET, SOCK_DGRAM, TCP_MODE);
    
    // Create server address
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, CONNECTION_ADDRESS, &sv_addr.sin_addr);
    
    addrlen = sizeof(sv_addr);
    printf("Listening on %s:%d\n", CONNECTION_ADDRESS, SERVER_PORT);

    // Loop infinitely waiting for the server to send data
    for (int i=0; i < 2; i++)
    {
        // Set the current time recorded by the client
        time(&cl_timestamp);
        // Send the current time to the server
        sent_bytes = sendto(sd, &cl_timestamp, sizeof(time_t), 0, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
        if (sent_bytes == -1)
        {
            fprintf(stderr, "Failed to send bytes");
            exit(EXIT_FAILURE);
        }
        // Print the time the request was sent
        printf("Sent timestamp '%ld' to server.\n", (long)cl_timestamp);
        // Wait for a response for the servers' timestamp
        received_bytes = recvfrom(sd, &sv_timestamp, sizeof(time_t), MSG_WAITALL, (struct sockaddr*)&sv_addr, &addrlen);
        if (received_bytes == -1)
        {
            fprintf(stderr, "Failed to receive bytes");
            exit(EXIT_FAILURE);
        }
        // Print the time the server sent back
        printf("Recieved timestamp:'%ld' from server.\n", (long)sv_timestamp);
    }
    // Close the socket
    close(sd);


    return 0;
}
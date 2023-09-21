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
    ssize_t received_bytes;
    time_t timestamp;

    // Create socket
    sd = socket(AF_INET, SOCK_STREAM, TCP_MODE);
    
    // Create server address
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, CONNECTION_ADDRESS, &sv_addr.sin_addr);
    
    addrlen = sizeof(sv_addr);

    // Connect to server
    ret = connect(sd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    // Check for connection errors
    if (ret == -1) {
        // If connection fails, exit
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Loop infinitely waiting for the server to send data
    for (int i=0; i < 2; i++)
    {
        // Wait for data to be received
        received_bytes = recv(sd, &timestamp, sizeof(time_t), 0);
        // Check data was received successfully
        if (received_bytes == -1) {
            // If data failed, close the socket and exit 
            perror("recv");
            close(sd);
            exit(EXIT_FAILURE);
        } else if (received_bytes == 0) {
            // If the recieved bytes == 0, print the server connection has been closed
            printf("Server closed the connection.\n");
            break;
        } else {
            // Successfully received the timestamp
            printf("Timestamp received: %ld\n", (long)timestamp);
        }
        //sleep(5);
        
    }
    // Close the socket
    close(sd);


    return 0;
}
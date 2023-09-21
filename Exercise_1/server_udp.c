#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>


#define CONNECTION_ADDRESS "127.0.0.1"
#define SERVER_PORT 18000
#define BACKLOG_SIZE 10
#define TCP_MODE 0
#define UPDATE_INTERVAL 5

/*
// Struct format
struct sockaddr_in {
    // sin = sockaddr_in -> s_in -> sin
    sa_family_t sin_family; // address family: AF_INET
    in_port_t sin_port; // port in network byte order
    struct in_addr sin_addr; // internet address
};

// Internet address
struct in_addr {
    uint32_t s_addr; // address in network byte order
};

*/
int main()
{
    
    // This is for when the client disconnects, it causes a broken pipe which terminates the entire program (this is only when I am not using child processes)
    // This is happening because the server is sleeping for 5 seconds and the client is not waiting for the result of its final request before disconnecting
    //signal(SIGPIPE, SIG_IGN);

    // The fix is to use child processes and the parent can deal with the connection and the children can deal with processing

    
    int ret, sd, cl_sd;
    struct sockaddr_in my_addr, cl_addr;
    socklen_t addrlen = sizeof(cl_addr);
    ssize_t sent_bytes, received_bytes;
    bool active_client = false;
    time_t sv_timestamp, cl_timestamp;

    // Create socket
    sd = socket(AF_INET, SOCK_DGRAM, TCP_MODE);
    // Check successful socket creation

    // Create address
    memset(&my_addr, 0, sizeof(my_addr)); // sets the first count bytes of dest to the value c
    my_addr.sin_family = AF_INET; // set the sockadress transport address to AF_INET (Address Family InterNET)
    my_addr.sin_port = htons(SERVER_PORT); // converts unsigned short integer (hostshort) from host byte order to netword byte order
    inet_pton(AF_INET, CONNECTION_ADDRESS, &my_addr.sin_addr); // converts IPv4 and IPv6 addresses from text to binary form

    // assigns the local socket address to a socket identified by descriptor socket that has no local socket address assigned
    // sockets created with socket() are initially unnamed; they are identified by their address family
    ret = bind(sd, (struct sockaddr*)&my_addr, sizeof(my_addr)); 

    printf("Listening on %s:%d\n", CONNECTION_ADDRESS, SERVER_PORT);

    while (1)
    {

        // Get the request from the client
        received_bytes = recvfrom(sd, &cl_timestamp, sizeof(time_t), MSG_WAITALL, (struct sockaddr*)&cl_addr, &addrlen);
        // Check data was sent correctly
        if (received_bytes == -1)
        {
            fprintf(stderr, "Failed to receive bytes");
            exit(EXIT_FAILURE);
        }
        // Print the recieved data from the client
        printf("Recieved timestamp:'%ld' from client.\n", (long)cl_timestamp);
        // Sleep for UPDATE_INTERVAL seconds
        sleep(UPDATE_INTERVAL);
        // Set the time recorded by the server
        time(&sv_timestamp);        
        // Send the time recorded from the server to the client
        sent_bytes = sendto(sd, &sv_timestamp, sizeof(time_t), 0, (struct sockaddr*)&cl_addr, sizeof(cl_addr));
        if (sent_bytes == -1)
        {
            fprintf(stderr, "Failed to send bytes");
            exit(EXIT_FAILURE);
        }
        // Print that the data was sent
        printf("Sent timestamp '%ld' to client.\n", (long)sv_timestamp);

    }
    // Close the main socket
    close(sd);

    return 0;
}
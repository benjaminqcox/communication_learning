#include <sys/types.h>
#include <sys/socket.h>
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
    ssize_t sent_bytes;
    bool active_client = false;

    // Create socket
    sd = socket(AF_INET, SOCK_STREAM, TCP_MODE);
    // Check successful socket creation

    // Create address
    memset(&my_addr, 0, sizeof(my_addr)); // sets the first count bytes of dest to the value c
    my_addr.sin_family = AF_INET; // set the sockadress transport address to AF_INET (Address Family InterNET)
    my_addr.sin_port = htons(SERVER_PORT); // converts unsigned short integer (hostshort) from host byte order to netword byte order
    inet_pton(AF_INET, CONNECTION_ADDRESS, &my_addr.sin_addr); // converts IPv4 and IPv6 addresses from text to binary form

    // assigns the local socket address to a socket identified by descriptor socket that has no local socket address assigned
    // sockets created with socket() are initially unnamed; they are identified by their address family
    ret = bind(sd, (struct sockaddr*)&my_addr, sizeof(my_addr)); 
    ret = listen(sd, BACKLOG_SIZE); // listen on the created socket with a maximum backlog size of 10
    // Check the listen was successfully setup
    if (ret == -1) {
        // If failed, exit the program
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Print port listening on
    printf("Server is listening on port %d...\n", SERVER_PORT);

    while (1)
    {
        // extract the first connection request in the queue of pending connections, create a new socket with the same socket type protocol
        // and address family as the specified socket, and allocate a new file descriptor for that socket
        cl_sd = accept(sd, (struct sockaddr*)&cl_addr, &addrlen);
        // Check client connection was made successfully
        if (cl_sd == -1) {
            // If not successful, loop back to wait for new connection
            perror("accept");
            continue;
        }
        
        if (fork() == 0)
        {
            close(sd);
            // Set the connection to the client as true
            active_client = true;
            // Loop while the client is active
            while (1)
            {
                time_t curr_datetime;
                time(&curr_datetime);
                // Issue here, server will always send 1 too many packets as the active client won't get set to false until after this has been called as active client is set below
                // How do I fix this???
                sent_bytes = send(cl_sd, &curr_datetime, sizeof(time_t), MSG_NOSIGNAL);
                if (sent_bytes == 0) {
                    // If send fails, exit loop
                    perror("send");
                    break;
                }
                else if (sent_bytes == -1)
                {
                    printf("Client closed the connection.\n");
                    break;
                }
                // If data sent successfully, sleep and send again
                printf("Sent timestamp '%ld' to client.\n", curr_datetime);
                sleep(UPDATE_INTERVAL);
            }
            close(cl_sd);
            exit(EXIT_SUCCESS);
        }
        else
        {
           // Close client connection if internal loop exits
            close(cl_sd); 
        }
        
    }
    // Close the main socket
    close(sd);

    return 0;
}
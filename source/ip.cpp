#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

string getIP()
{
    char host[256];
    char *IP;
    struct hostent *host_entry;
    int hostname;
    hostname = gethostname(host, sizeof(host)); //find the host name
    if (hostname == -1) 
    {
      perror("gethostname");
      exit(1);
    }
    host_entry = gethostbyname(host); //find host information
    if (host_entry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
    // printf("Current Host Name: %s\n", host);
    // printf("Host IP: %s\n", IP);
    return to_string(IP);
}

int main(void) 
{
    hostandIP();
    return 0;
}
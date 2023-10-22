#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> //to create a socket, to use bind()
#include <arpa/inet.h>
#include <netinet/in.h> //defines a struct type sockaddr_in. unless you define it yourself

using namespace std;

#define BUFFSIZE 1024

int main(){
    char buffer[BUFFSIZE];

    /* start by promting user for sever host name and server port number */
    string hostName;
    cout << "Enter the server host name: " << endl;
    cin >> hostName;

    int port;
    cout << "Enter the server port number: " << endl;
    cin >> port;
    
    /* create datagram socket, and prompt user for a car license plate */
    int clientSocket;
    if((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        cout << "Client Socket creation failed" << endl;
        return -1;
    }

    struct sockaddr_in servaddr;

    memset(&servaddr, 0 , sizeof(servaddr));

    //  fill server information
    servaddr.sin_family = AF_INET;          // set address family = internet
    servaddr.sin_addr.s_addr = INADDR_ANY;  // set IP address to localhost
    servaddr.sin_port = htons(port);        // set port number, htons function to use proper byte order

    socklen_t len;
    cout << "Enter a license plate number: " << endl;
    //fgets(buffer, 1024, stdin);
    scanf("%s", buffer);
    buffer[strcspn(buffer, "\n")] = 0;  //gets rid of the \n character
    int nBytes = strlen(buffer)+ 1;

    //sending license plate
    if(sendto(clientSocket, buffer, nBytes, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr))
        != -1 ){
        //cout << "Message sent (not gurantee delivery)"  << endl;
    }

    //get feedback from server
    nBytes = recvfrom(clientSocket, buffer, BUFFSIZE, 0, NULL, NULL);
    printf("%s\n", buffer);


    return 0;
}
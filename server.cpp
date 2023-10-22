#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> //to create a socket, to use bind()
#include <arpa/inet.h>
#include <netinet/in.h> //defines a struct type sockaddr_in. unless you define it yourself
#include <fstream>
#include <string>
#include <vector>
using namespace std;
#define BUFFSIZE 1024

void readData(ifstream &fin, vector<string> &plates);   //read txt file, stores info into vector
bool findPlate(vector<string> &plate, char buffer[]);   //search license plate, returns true if found




int main(){
    vector<string> licensePlates;
    
    //Server will prompt user to enter the name of database of stolen cars
    cout << "Enter today's stolen car DB name (e.g. database.txt):" << endl;
    /*  C-code to read stdin
    char *database = NULL;
    size_t dataBaseLen = 0;
    ssize_t databaseSize = 0;
    databaseSize = getline(&database, &dataBaseLen, stdin);
    printf("You entered %s, and has %zu chars.\n", database, databaseSize - 1);
    */
    string dbName;
    cin >> dbName;
    ifstream fin(dbName);
    if(!fin.is_open()){
        cout << "Can't find database... ending program" << endl;
        return -1;
    }
    readData(fin, licensePlates);   //read data and store it

    //server will prompt for a port number.
    int port;
    cout << "Enter the server port number: ";
    cin >> port;
    cout << endl;

    //it will then:
    /*  1. create datagram socket in the internet domain
            int socket(specifies communications domain of socket is created IPv4/IPv6, type of socket TCP/UDP, default protocol = 0)
    */
    int serverSocket;
    if((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        cout << "Server Socket Creation Failed" << endl;
        return -1;
    }

    /*  2. do a bind() to bind to the socket to the specified port number
            int bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
    */
    struct sockaddr_in servaddr;   //include netinet/in.h or create own structure
    struct sockaddr_in cliaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    servaddr.sin_family = AF_INET;          // set address family = internet
    servaddr.sin_addr.s_addr = INADDR_ANY;  // set IP address to localhost
    servaddr.sin_port = htons(port);        // set port number, htons function to use proper byte order

    // bind the address struct to the socket
    if(::bind(serverSocket, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){ /* ::bind() if using namespace std; */
        cout << "Bind to Socket Fail" << endl;
        close(serverSocket);
        return -1;
    }


    /*  3. enter infinite loop, repeatedly do a recvfrom() then sendto() to let each client know if given license plate is in the database. 
            #include <sys/socket.h>

            ssize_t recvfrom(int socket, void *restrict buffer, size_t length,
                    int flags, struct sockaddr *restrict address,
                    socklen_t *restrict address_len);

            ssize_ t sendto(int socket, const void *message, size_t length,
                    int flags, const struct sockaddr *dest_addr,
                    socklen_t dest_len);
    */

    char buffer[BUFFSIZE];
    socklen_t len = sizeof(cliaddr);    //is value/result
    int nBytes;

    while(true)  //loop should end when server gets KILLSVC message from one of clients
    {

        nBytes = recvfrom(serverSocket, buffer, BUFFSIZE, 0, (struct sockaddr *) &cliaddr, &len);  // flag values where set to MSG_WAITALL
        buffer[nBytes] = '\0';
        printf("\tClient : %s\n", buffer);
        char msg[BUFFSIZE]; //use this to fill message, then send it to client

        //check client's request
        if(memcmp(buffer, "killsvc", strlen("killsvc")) == 0)
        {
            strcpy(msg, ": Server recieved 'killsvc'... ending server");
            strcat(buffer, msg);
            nBytes = strlen(buffer) + 1;
            if(sendto(serverSocket, buffer, nBytes, 0, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) 
                    != -1)
                {
                    //cout << "Server message sent (not gurantee delivery)" << endl;
                }
            break;
        }
        else
        {
            if(findPlate(licensePlates, buffer))
            {
                strcpy(msg, ": Reported as stolen");
                strcat(buffer, msg);
                nBytes =  strlen(buffer) + 1;
                if(sendto(serverSocket, buffer, nBytes, 0, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) 
                    != -1)
                {
                    //cout << "Server message sent (not gurantee delivery)" << endl;
                }
            }
            else
            {
                strcpy(msg, ": Not in the database");
                strcat(buffer, msg);
                nBytes =  strlen(buffer) + 1;
                if(sendto(serverSocket, buffer, nBytes, 0, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) 
                    != -1)
                {
                    //cout << "Server message sent (not gurantee delivery)" << endl;
                }
            }
        }
    }

    cout << "Server shutdown" << endl;
    fin.close();
    return 0;
}


void readData(ifstream &fin, vector<string> &plates)
{
    string temp;
    while((fin >> temp)){
        plates.push_back(temp);
        //cout << "\t" << temp << endl;   //print txt file out
    }
}
bool findPlate(vector<string> &plate, char buffer[]){
    for(int i = 0; i < plate.size(); i++){
        if(memcmp(buffer, plate.at(i).c_str(), strlen(plate.at(i).c_str())) == 0){
            return true;
        }
    }
    return false;
}
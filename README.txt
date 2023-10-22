MacOS

Server file:
    When asked for the file name containing the license plate information,
    you must include the .txt after the name. Example: LicensePlates.txt   
    To compile: g++ -o serverTest server.cpp
                ./serverTest

Client file:
    When asked for the host name, type: localhost
    When you want to close the server, you must type in: killsvc
        it is case sensitive.
    To compile: g++ -o clientTest client.cpp
                ./clientTest

You must run the server first, and then the client.
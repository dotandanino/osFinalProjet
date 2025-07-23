#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "Graph.hpp"
#include "algoFactory.hpp"
#define PORT 8080
#define BUFFERSIZE 4096
using namespace std;

int main(){
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // create the socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // set socket options to allow reuse of the address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // connect the socket to the address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // create a queue with 10 places for the socket to listen for incoming connections
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    int buffer[BUFFERSIZE / sizeof(int)];
    for(;;){
        int byteRecv = recv(new_socket,buffer,BUFFERSIZE,0);
        if(byteRecv==0){
            cout << "Client disconnected." << endl;
            close(new_socket);
            break;
        }
        int n=buffer[0];
        vector<vector<int>> neighborsMatrix(n, vector<int>(n, -1));
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                neighborsMatrix[i][j]=buffer[i*n+j+1];
            }
        }
        Graph g(n,neighborsMatrix);
        int algoType = buffer[n*n+1];
        std::unique_ptr<GraphAlgorithm> algorithm = createAlgorithm(static_cast<AlgorithmType>(algoType));
        if (algorithm == nullptr) {
            std::string err = "Unknown algorithm type";
            send(new_socket, err.c_str(), err.size(), 0);
            continue;
        }
        std::string result = algorithm->execute(g);
        send(new_socket, result.c_str(), result.size(), 0);
    }
}
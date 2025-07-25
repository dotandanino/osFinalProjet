#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include "../question7/Graph.hpp"
#include "../question7/algoFactory.hpp"
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <algorithm>

#define PORT 8080
#define BUFFERSIZE 4096
using namespace std;
#define N 5
std::mutex mtx; // Mutex for thread safety
std::condition_variable cv;
int fd_count = 2;
// a vector to hold the file descriptors for polling
vector<pollfd> fds(fd_count);

std::queue<int> q; // Queue to hold file descriptors

std::unique_ptr<GraphAlgorithm> algo1 = createAlgorithm(static_cast<AlgorithmType>(1));
std::unique_ptr<GraphAlgorithm> algo2 = createAlgorithm(static_cast<AlgorithmType>(2));
std::unique_ptr<GraphAlgorithm> algo3 = createAlgorithm(static_cast<AlgorithmType>(3));
std::unique_ptr<GraphAlgorithm> algo4 = createAlgorithm(static_cast<AlgorithmType>(4));

bool server_running = true; // Flag to control server running state

Graph createRandomGraph(int numOfVertex, int numOfEdges, int RandomSeed,int minWeight, int maxWeight) {
    unsigned long maxEdge = (numOfVertex -1)*numOfVertex;
    if((unsigned long)numOfEdges>maxEdge){
        std::cerr<<"E"<<numOfEdges<<" maxEdge "<<maxEdge<<std::endl; 
        std::cerr<<"ypu enter too many edges"<<std::endl;
        exit(1);
    }
    int edgesAdded=0;
    Graph g(numOfVertex);
    srand(RandomSeed);
    while(edgesAdded<numOfEdges){
        int src=rand()%numOfVertex;
        int dest =rand()%numOfVertex;
        int weight = rand() % (maxWeight - minWeight + 1) + minWeight;
        try{
            g.addDirectedEdge(src,dest,weight);
            edgesAdded++;
        }catch(const std::exception& e){
            //nothing to do here just wait for another graph
        }
    }
    g.printGraph();
    return g;
}

void clientHandle(){
    while(server_running) {
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []{ return !q.empty() || !server_running; });
            cout << "finished wait"<<endl;
            if(!q.empty()) {
                fd = q.front();
                q.pop();
            }
        }
        if(!server_running && fd == -1) {
            cout << "Server is not running and no file descriptor is available." << endl;
            break; // Exit if server is not running and no file descriptor is available
        }
        if (fd == -1) {
            continue;
        }
        int buffer[BUFFERSIZE / sizeof(int)];
        int byteRecv = recv(fd,buffer,BUFFERSIZE,0);
        if(byteRecv==0){
            cout << "Client disconnected." << endl;
            close(fd);
            auto it = std::remove_if(fds.begin(), fds.end(), [fd](const pollfd& p) {
                return p.fd == fd;
            });
            fds.erase(it, fds.end());
            break;
        }
        int n=buffer[0];
        if(n == 0) {
            cout << "Random graph selected." << endl;
            int numOfVertex = buffer[1];
            int numOfEdges = buffer[2];
            int RandomSeed = buffer[3];
            int minWeight = buffer[4];
            int maxWeight = buffer[5];
            if (minWeight > maxWeight) {
                string e = "Error: minWeight cannot be greater than maxWeight";
                send(fd, e.c_str(), e.size(), 0);
                continue;
            }
            cout << "Creating random graph with " << numOfVertex << " vertices, " << numOfEdges << " edges, and random seed " << RandomSeed << endl;
            Graph g = createRandomGraph(numOfVertex, numOfEdges, RandomSeed,minWeight, maxWeight);
            cout<<"Before MaxFlow"<<endl;
            std::string result = algo1->execute(g);
            cout<<"Before MST"<<endl;
            result += "\n" + algo2->execute(g);
            cout<<"Before PathCover"<<endl;
            result += "\n" + algo3->execute(g);
            cout<<"Before SCC"<<endl;
            result += "\n" + algo4->execute(g);
            cout << "Sending results to client..." << endl;
            send(fd, result.c_str(), result.size(), 0);
            continue;
        }
        vector<vector<int>> neighborsMatrix(n, vector<int>(n, -1));
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                neighborsMatrix[i][j]=buffer[i*n+j+1];
            }
        }
        Graph g(n,neighborsMatrix);
        cout<<"Before MaxFlow"<<endl;
        std::string result = algo1->execute(g);
        cout<<"Before MST"<<endl;
        result += "\n" + algo2->execute(g);
        cout<<"Before PathCover"<<endl;
        result += "\n" + algo3->execute(g);
        cout<<"Before SCC"<<endl;
        result += "\n" + algo4->execute(g);
        cout << "Sending results to client..." << endl;
        send(fd, result.c_str(), result.size(), 0);
    }
}

int main(){
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    std::thread threads[4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = std::thread(clientHandle); // Create threads for handling clients
    }


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
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO; // Add stdin to the poll list
    fds[1].events = POLLIN;

    int buffer[BUFFERSIZE / sizeof(int)];
    for(;;){
        int poll_count = poll(fds.data(), fd_count, -1);
        if (poll_count < 0) {
            perror("poll failed");
            exit(EXIT_FAILURE);
        }
        for(int i=0;i<fd_count;i++){
            if(fds[i].revents & POLLIN){
                if(fds[i].fd == server_fd){
                    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    fds.push_back({new_socket, POLLIN});
                    fd_count++;
                }else if(fds[i].fd == STDIN_FILENO){
                    // Handle input from stdin
                    string input;
                    getline(cin, input);
                    cout<<"input is "<<input<<endl;
                    if(input == "exit") {
                        server_running = false;
                        for(int j=2; j<fd_count; j++) {
                            close(fds[j].fd); // Close all client sockets
                        }
                        close(server_fd); // Close the server socket
                        server_running = false; // Set the server running flag to false
                        cv.notify_all(); // Notify all waiting threads to exit
                        cout<<"Server shutting down..."<<endl;
                        for(int i=0;i<4;i++){
                            threads[i].join(); // Wait for all threads to finish
                        }
                        for(int i=0;i<fd_count;i++){
                            if(fds[i].fd != -1) {
                                close(fds[i].fd); // Close all file descriptors
                            }
                        }
                        exit(0);
                    }
                }
                else{
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        q.push(fds[i].fd);
                        cv.notify_one();
                    }
                }
            }
        }
    }
}
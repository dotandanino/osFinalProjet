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
int server_fd;
std::mutex mtx;
std::condition_variable cv;
bool hasLeader = false;
bool server_running = true;

vector<pollfd> fds; 

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
            g.addDirectedEdge(src,dest  ,weight);
            edgesAdded++;
        }catch(const std::exception& e){
            //nothing to do here just wait for another graph
        }
    }
    return g;
}

void clientHandle(int fd){
    cout<<"connect to "<<fd<<endl;
    std::unique_ptr<GraphAlgorithm> algo1 = createAlgorithm(static_cast<AlgorithmType>(1));
    std::unique_ptr<GraphAlgorithm> algo2 = createAlgorithm(static_cast<AlgorithmType>(2));
    std::unique_ptr<GraphAlgorithm> algo3 = createAlgorithm(static_cast<AlgorithmType>(3));
    std::unique_ptr<GraphAlgorithm> algo4 = createAlgorithm(static_cast<AlgorithmType>(4));
    if(!server_running) {
        return; // Exit if server is not running and no file descriptor is available
    }
    int buffer[BUFFERSIZE / sizeof(int)];
    int byteRecv = recv(fd,buffer,BUFFERSIZE,0);
    if(byteRecv==0){
        close(fd);
        auto it = std::remove_if(fds.begin(), fds.end(), [fd](const pollfd& p) {
            return p.fd == fd;
        });
        fds.erase(it, fds.end());
        return;
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
                return;
        }
        cout << "Creating random graph with " << numOfVertex << " vertices, " << numOfEdges << " edges, and random seed " << RandomSeed << endl;
        Graph g = createRandomGraph(numOfVertex, numOfEdges, RandomSeed,minWeight, maxWeight);
        std::string result = algo1->execute(g);
        result += "\n" + algo2->execute(g);
        result += "\n" + algo3->execute(g);
        result += "\n" + algo4->execute(g);
        cout << "Sending results to client..." << endl;
        send(fd, result.c_str(), result.size(), 0);
        return;
    }
    vector<vector<int>> neighborsMatrix(n, vector<int>(n, -1));
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            neighborsMatrix[i][j]=buffer[i*n+j+1];
        }
    }
    Graph g(n,neighborsMatrix);
    std::string result = algo1->execute(g);
    result += "\n" + algo2->execute(g);
    result += "\n" + algo3->execute(g);
    result += "\n" + algo4->execute(g);
    cout << "Sending results to client..." << endl;
    send(fd, result.c_str(), result.size(), 0);
}

void worker(int id) {
    while (server_running) {
        unique_lock<mutex> lock(mtx);

        cv.wait(lock, []{ return !hasLeader || !server_running; });//wait untill there is no leader
        if (!server_running) break;

        //become the leader
        hasLeader = true;
        lock.unlock();

        // Leader waits for an event
        int poll_count = poll(fds.data(), fds.size(), 5000);

        // Once I found an event, I release the role
        lock.lock();
        hasLeader = false;
        cv.notify_one(); // Wake up another thread to become Leader
        lock.unlock();

        if (poll_count < 0) {
            perror("poll failed");
            break;
        }

        // I am already a Follower, so I handle the event myself
        for (size_t i = 0; i < fds.size(); i++) {
            if(!server_running) {
                return; // Exit if server is not running
            }
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == fds[0].fd) {
                    // Event on server_fd -> accept
                    struct sockaddr_in address;
                    socklen_t addrlen = sizeof(address);
                    int new_socket = accept(fds[0].fd, (struct sockaddr*)&address, &addrlen);
                     if (errno == EBADF || errno == EINVAL) {// Check if the socket is closed
                        cout << "Server socket closed" << endl;
                        return;
                    }
                    if (new_socket >= 0) {
                        cout << "New client accepted: " << new_socket << endl;
                        lock_guard<mutex> l(mtx);
                        fds.push_back({new_socket, POLLIN, 0});
                    }
                } else if (fds[i].fd == STDIN_FILENO) {
                    string input;
                    getline(cin, input);
                    if (input == "exit") {
                        lock_guard<mutex> l(mtx);
                        server_running = false;
                        shutdown(server_fd, SHUT_RDWR);
                        for (size_t j = 2; j < fds.size(); j++) {
                            close(fds[j].fd); // Close all client sockets
                        }
                        cv.notify_all();
                        cout << "Server shutting down..." << endl;
                        return;
                    }
                } else {
                    clientHandle(fds[i].fd);
                }
            }
        }
    }
}
int main(){
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

    fds.push_back({server_fd, POLLIN, 0});
    fds.push_back({STDIN_FILENO, POLLIN, 0});

    const int N = 4;
    vector<thread> threads;
    for (int i = 0; i < N; i++) {
        threads.emplace_back(worker, i+1);
    }

    {
        lock_guard<mutex> lock(mtx);
        hasLeader = false;
        cv.notify_one();
    }

    for (auto &t : threads) t.join();

    close(server_fd);
    return 0;
}
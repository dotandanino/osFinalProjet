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
#include <atomic>

#define PORT 8080
#define BUFFERSIZE 4096
using namespace std;
#define N 5
std::mutex mtx1,mtx2,mtx3,mtx4,mtx5,mtx6; // Mutex for thread safety
std::condition_variable cv1,cv2,cv3,cv4,cv5,cv6; // Condition variables for signaling

int fd_count = 2;
vector<pollfd> fds(fd_count);


struct toqueue {
    int fd;
    std::string result;
    Graph g;
};
std::queue<int> q1; // Queue to hold file descriptors
std::queue<toqueue> q2,q3,q4,q5,q6;

std::unique_ptr<GraphAlgorithm> algo1 = createAlgorithm(static_cast<AlgorithmType>(1));
std::unique_ptr<GraphAlgorithm> algo2 = createAlgorithm(static_cast<AlgorithmType>(2));
std::unique_ptr<GraphAlgorithm> algo3 = createAlgorithm(static_cast<AlgorithmType>(3));
std::unique_ptr<GraphAlgorithm> algo4 = createAlgorithm(static_cast<AlgorithmType>(4));

Graph createRandomGraph(int numOfVertex, int numOfEdges, int RandomSeed,int minWeight, int maxWeight) {
    unsigned long maxEdge = (numOfVertex -1)*numOfVertex;
    if((unsigned long)numOfEdges>maxEdge){
        throw std::runtime_error("You entered too many edges");
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
    return g;
}

std::atomic<bool> server_running(true); // Flag to control server running state

void maxFlow(){
    while(server_running) {
        Graph g(1);//temp
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx2);
            cv2.wait(lock, []{ return !q2.empty() || !server_running;}); // Wait until there is something in the queue
            if(!q2.empty()) {
                auto item = q2.front();
                q2.pop();
                fd = item.fd;
                g = item.g;
            }
        }
        if(fd == -1 && !server_running) {
            break; // Exit if server is not running and no file descriptor is available
        }
        if (fd == -1) {
            continue;
        }
        std::string result = algo1->execute(g);
        {
            std::lock_guard<std::mutex> lock(mtx3);
            q3.push({fd, result, g});
            cv3.notify_one();
        }
    }
}

void MST(){
    while(server_running) {
        Graph g(1);//temp
        std::string result;
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx3);
            cv3.wait(lock, []{ return !q3.empty() || !server_running;}); // Wait until there is something in the queue
            if(!q3.empty()) {
                auto item = q3.front();
                q3.pop();
                fd = item.fd;
                g = item.g;
                result = item.result;
            }
        }
        if(fd == -1 && !server_running) {
            break; // Exit if server is not running and no file descriptor is available
        }
        if (fd == -1) {
            continue;
        }
        result +="\n";
        result += algo2->execute(g);
        {
            std::lock_guard<std::mutex> lock(mtx4);
            q4.push({fd, result, g});
            cv4.notify_one();
        }
    }
}

void PathCover(){
    while(server_running) {
        Graph g(1);//temp
        std::string result;
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx4);
            cv4.wait(lock, []{ return !q4.empty() || !server_running; }); // Wait until there is something in the queue
            if(!q4.empty()) {
                auto item = q4.front();
                q4.pop();
                fd = item.fd;
                g = item.g;
                result = item.result;
            }
        }
        if(fd == -1 && !server_running) {
            break; // Exit if server is not running and no file descriptor is available
        }
        if (fd == -1) {
            continue;
        }
        result += "\n";
        result += algo3->execute(g);
        {
            std::lock_guard<std::mutex> lock(mtx5);
            q5.push({fd, result, g});
            cv5.notify_one();
        }
    }
}


void SCC(){
    while(server_running) {
        Graph g(1);//temp
        std::string result;
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx5);
            cv5.wait(lock, []{ return !q5.empty() || !server_running; }); // Wait until there is something in the queue
            if(!q5.empty()) {
                auto item = q5.front();
                q5.pop();
                fd = item.fd;
                g = item.g;
                result = item.result;
            }
        }
        if(fd == -1 && !server_running) {
            break; // Exit if server is not running and no file descriptor is available
        }
        if (fd == -1) {
            continue;
        }
        result += "\n";
        result += algo4->execute(g);
        {
            std::lock_guard<std::mutex> lock(mtx6);
            q6.push({fd, result, g});
            cv6.notify_one();
        }
    }
}


void senderHandle(){
    while(server_running) {
        int fd=-1;
        string result;
        {
            std::unique_lock<std::mutex> lock(mtx6);
            cv6.wait(lock, []{ return !q6.empty() || !server_running;}); // Wait until there is something in the queue
            if(!q6.empty()) {
                toqueue item = q6.front();
                fd = item.fd;
                result = item.result;
                q6.pop();
            }
        }
        if(fd == -1 && !server_running) {
            break; // Exit if server is not running and no file descriptor is available
        }
        if (fd == -1) {
            continue;
        }
        send(fd, result.c_str(), result.size(), 0);
    }
}


int main(){
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    std::thread threads[5];
    threads[0] = std::thread(maxFlow);
    threads[1] = std::thread(MST);
    threads[2] = std::thread(PathCover);
    threads[3] = std::thread(SCC);
    threads[4] = std::thread(senderHandle);

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
    // a vector to hold the file descriptors for polling
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
                }
                else if(fds[i].fd == STDIN_FILENO){
                    // Handle input from stdin
                    string input;
                    cin >> input;
                    if(input == "exit") {
                        server_running = false;
                        for(int j=2; j<fd_count; j++) {
                            close(fds[j].fd); // Close all client sockets
                        }
                        close(server_fd); // Close the server socket
                        server_running = false; // Set the server running flag to false
                        {
                            std::lock_guard<std::mutex> lock(mtx1);
                            cv1.notify_all(); // Notify all waiting threads to exit
                        }
                        {
                            std::lock_guard<std::mutex> lock(mtx2);
                            cv2.notify_all();
                        }
                        {
                            std::lock_guard<std::mutex> lock(mtx3);
                            cv3.notify_all();
                        }
                        {
                            std::lock_guard<std::mutex> lock(mtx4);
                            cv4.notify_all();
                        }
                        {
                            std::lock_guard<std::mutex> lock(mtx5);
                            cv5.notify_all();
                        }
                        {
                            std::lock_guard<std::mutex> lock(mtx6);
                            cv6.notify_all();
                        }
                        for(int i=0;i<5;i++){
                            threads[i].join(); // Wait for all threads to finish
                        }
                        exit(0);
                    }
                }
                else{
                    int byteRecv = recv(fds[i].fd,buffer,BUFFERSIZE,0);
                    if(byteRecv==0){
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        fd_count--;
                        continue;
                    }
                    int n=buffer[0];
                    if(n == 0) {
                        int numOfVertex = buffer[1];
                        int numOfEdges = buffer[2];
                        int RandomSeed = buffer[3];
                        int minWeight = buffer[4];
                        int maxWeight = buffer[5];
                        if (minWeight > maxWeight) {
                            string e = "Error: minWeight cannot be greater than maxWeight";
                            send(fds[i].fd, e.c_str(), e.size(), 0);
                            continue;
                        }
                        try{
                            Graph g = createRandomGraph(numOfVertex, numOfEdges, RandomSeed,minWeight, maxWeight);
                            {
                                std::lock_guard<std::mutex> lock(mtx2);
                                q2.push({fds[i].fd, "", g});
                                cv2.notify_one();
                            }
                        }
                        catch(const std::exception& e){
                            string errorMsg = "Error creating random graph: " + std::string(e.what());
                            send(fds[i].fd, errorMsg.c_str(), errorMsg.size(), 0);
                        }
                        continue;
                    }

                    vector<vector<int>> neighborsMatrix(n, vector<int>(n, -1));
                    for(int i=0;i<n;i++){
                        for(int j=0;j<n;j++){
                            neighborsMatrix[i][j]=buffer[i*n+j+1];
                        }
                    }
                    Graph g(n,neighborsMatrix);
                    {
                        std::lock_guard<std::mutex> lock(mtx2);
                        q2.push({fds[i].fd, "", g});
                        cv2.notify_one();
                    }
                }
            }
        }
    }
}
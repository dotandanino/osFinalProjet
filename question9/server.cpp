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
#include <unordered_map>

#define PORT 8080
#define BUFFERSIZE 4096
using namespace std;
#define N 5
std::mutex mtx1,mtx2,mtx3,mtx4,mtx5,mtx6; // Mutex for thread safety
std::condition_variable cv1,cv2,cv3,cv4,cv5,cv6; // Condition variables for signaling

std::unordered_map<int, bool> alreadyInQueue;// to know which client already in the queue

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
    return g;
}

void recvData(){
    while(true) {
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx1);
            cv1.wait(lock, []{ return !q1.empty(); }); // מחכה עד שיש משהו בתור
            fd = q1.front();
            q1.pop();
            alreadyInQueue[fd] = false; // Mark the socket as not already in queue
        }
        if (fd == -1) {
            continue;
        }
        int buffer[BUFFERSIZE / sizeof(int)];
        int byteRecv = recv(fd,buffer,BUFFERSIZE,0);
        if(byteRecv==0){
            cout << "Client disconnected." << endl;
            close(fd);
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
            Graph g = createRandomGraph(numOfVertex, numOfEdges, RandomSeed,minWeight, maxWeight);
            {
                std::lock_guard<std::mutex> lock(mtx2);
                q2.push({fd, "", g});
                cv2.notify_one();
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
            q2.push({fd, "", g});
            cv2.notify_one();
        }
    }
}


void maxFlow(){
    while(true) {
        Graph g(1);//temp
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx2);
            cv2.wait(lock, []{ return !q2.empty(); }); // Wait until there is something in the queue
            auto item = q2.front();
            q2.pop();
            fd = item.fd;
            g = item.g;
        }
        if (fd == -1) {
            continue;
        }
        cout << "MaxFlow algorithm started." << endl;
        std::string result = algo1->execute(g);
        cout << "MaxFlow algorithm finished." << endl;
        {
            std::lock_guard<std::mutex> lock(mtx3);
            q3.push({fd, result, g});
            cv3.notify_one();
        }
    }
}


void MST(){
    while(true) {
        Graph g(1);//temp
        std::string result;
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx3);
            cv3.wait(lock, []{ return !q3.empty(); }); // Wait until there is something in the queue
            auto item = q3.front();
            q3.pop();
            fd = item.fd;
            g = item.g;
            result = item.result;
        }
        if (fd == -1) {
            continue;
        }
        cout << "MST algorithm started." << endl;
        result +="\n";
        result += algo2->execute(g);
        cout << "MST algorithm finished." << endl;
        {
            std::lock_guard<std::mutex> lock(mtx4);
            q4.push({fd, result, g});
            cv4.notify_one();
        }
    }
}

void PathCover(){
    while(true) {
        Graph g(1);//temp
        std::string result;
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx4);
            cv4.wait(lock, []{ return !q4.empty(); }); // Wait until there is something in the queue
            auto item = q4.front();
            q4.pop();
            fd = item.fd;
            g = item.g;
            result = item.result;
        }
        if (fd == -1) {
            continue;
        }
        cout << "PathCover algorithm started." << endl;
        result += "\n";
        result += algo3->execute(g);
        cout << "PathCover algorithm finished." << endl;
        {
            std::lock_guard<std::mutex> lock(mtx5);
            q5.push({fd, result, g});
            cv5.notify_one();
        }
    }
}


void SCC(){
    while(true) {
        Graph g(1);//temp
        std::string result;
        int fd = -1;
        {
            std::unique_lock<std::mutex> lock(mtx5);
            cv5.wait(lock, []{ return !q5.empty(); }); // Wait until there is something in the queue
            auto item = q5.front();
            q5.pop();
            fd = item.fd;
            g = item.g;
            result = item.result;
        }
        if (fd == -1) {
            continue;
        }
        cout << "SCC algorithm started." << endl;
        result += "\n";
        result += algo4->execute(g);
        cout << "SCC algorithm finished." << endl;
        {
            std::lock_guard<std::mutex> lock(mtx6);
            q6.push({fd, result, g});
            cv6.notify_one();
        }
    }
}


void senderHandle(){
    while(true) {
        int fd=-1;
        string result;
        {
            std::unique_lock<std::mutex> lock(mtx6);
            cv6.wait(lock, []{ return !q6.empty(); }); // Wait until there is something in the queue
            toqueue item = q6.front();
            fd = item.fd;
            result = item.result;
            q6.pop();
        }
        if (fd == -1) {
            continue;
        }
        cout << "Sending results to client..." << endl;
        send(fd, result.c_str(), result.size(), 0);
    }
}


int main(){
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    std::thread threads[6];
    threads[0] = std::thread(recvData);
    threads[1] = std::thread(maxFlow);
    threads[2] = std::thread(MST);
    threads[3] = std::thread(PathCover);
    threads[4] = std::thread(SCC);
    threads[5] = std::thread(senderHandle);

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
    int fd_count = 1;
    int fd_capacity = 1;
    // a vector to hold the file descriptors for polling
    vector<pollfd> fds(fd_capacity);
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    
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
                    alreadyInQueue[new_socket] = false; // Initialize the new socket as not already in queue
                }
                else{
                    {
                        if(!alreadyInQueue[fds[i].fd]){
                            std::lock_guard<std::mutex> lock(mtx1);
                            q1.push(fds[i].fd);
                            alreadyInQueue[fds[i].fd] = true;
                            cv1.notify_one();
                        }
                    }
                }
            }
        }
    }
}
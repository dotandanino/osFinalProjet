#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
using namespace std;

int main(){
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Socket creation error" << endl;
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    
    if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid address / Address not supported" << endl;
        return -1;
    }
    

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection Failed" << endl;
        return -1;
    }
    int n =0 ;
    int src,dest,w;
    for(;;){
        cout<<"enter number of vertices if you want to exit enter -1 if you want random graph enter 0"<<endl;
        cin>>n;
        if(n == 0) {
            cout << "Random graph selected." << endl;
            cout<< "Enter number of vertices, edges, random seed, min weight , max weight: ";
            int numOfVertex, numOfEdges, RandomSeed, minWeight, maxWeight;
            cin >> numOfVertex >> numOfEdges >> RandomSeed >> minWeight >> maxWeight;
            int arr[6] = {n, numOfVertex, numOfEdges, RandomSeed, minWeight, maxWeight};
            send(sock, arr, sizeof(arr), 0);
        }
        else if(n==-1){
            close(sock);
            cout << "Connection closed." << endl;
            break;
        }
        else if(n<-1){
            cout<<"n must be positive number"<<endl;
            continue;
        }else{
            int* arr = new int[n*n+1]; //n*n for the neighbor matrix and extra place for the size
            arr[0]=n;
            for(int i=1;i<n*n+1;i++){
                arr[i]=-1; // initialize the matrix with -1
            }
            cout<<"now you will need to enter the neighbor matrix if you want to stop enter 0 for both src and dest"<<endl;
            for(;;){
                w=0;
                cout<<"enter src dest weight"<<endl;
                cin>>src>>dest>>w;
                if(dest==src && dest==0){
                    break;
                }
                if(dest<0 || dest>=n || src<0 || src>=n || src==dest || w<0){
                    cout<<"illegal arguments src,dest should different numbers be between 0 to n-1 and weight must not be negative"<<endl;
                    continue;
                }
                arr[src*n+dest+1]=w;//src*n + dest to change from matrix to arr +1 is because of the size
            }
            send(sock, arr,(n*n + 1)*sizeof(int), 0);
            cout << "Graph sent to server." << endl;
            delete[] arr;
        }
        char buffer[4096] = {0};
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::string received(buffer, bytesReceived);
            cout << received << endl;
        }
    }
}
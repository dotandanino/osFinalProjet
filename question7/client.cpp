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
        cout<<"enter number of vertices if you want to exit enter 0"<<endl;
        cin>>n;
        if(n==0){
            close(sock);
            cout << "Connection closed." << endl;
            break;
        }
        if(n<0){
            cout<<"n must be positive number"<<endl;
            continue;
        }
        int arr[n*n+2];//n*n for the neighbor matrix and extra place for the size and the algorithm type
        arr[0]=n;
        for(int i=1;i<n*n+1;i++){
            arr[i]=-1; // initialize the matrix with -1
        }
        cout<<"now you will need to enter the neighbor matrix if you want to stop enter 0 for both src and dest"<<endl;
        for(;;){
            cout<<"enter src dest"<<endl;
            cin>>src>>dest;
            if(dest==src && dest==0){
                break;
            }
            if(dest<0 || dest>=n || src<0 || src>=n || src==dest){
                cout<<"illegal arguments src,dest should different numbers be between 0 to n-1"<<endl;
                continue;
            }
            cout<<"enter weight"<<endl;
            cin>>w;
            arr[src*n+dest+1]=w;//src*n + dest to change from matrix to arr +1 is because of the size
        }
        cout << "Graph sent to server." << endl;
        cout << "Which algo do you want to run? \n1.maxflow\n2.mst\n3.pathcover\n4.scc"<<endl;
        int algo;
        cin>>algo;
        if(algo<1 || algo>4){
            cout<<"illegal algo number"<<endl;
            continue;
        }
        arr[n*n+1] = algo; // Store the selected algorithm type at the end of the array
        // Send the selected algorithm to the server
        send(sock, &arr,(n*n + 2)*sizeof(int), 0);
        char buffer[4096] = {0};
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::string receive(buffer, bytesReceived);
            cout<<receive<<endl;
        }
    }


}
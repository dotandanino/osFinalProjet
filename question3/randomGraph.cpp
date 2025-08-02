#include "../question2/Graph.hpp"
#include <iostream>
#include <unistd.h>

extern char* optarg;

int main(int argc, char* argv[]) {
    if( argc != 4){
    }
    int opt;
    int numOfVertex=-1;
    int numOfEdges=-1;
    int RandomSeed = -1;
    while ((opt = getopt(argc, argv, "v:e:r:")) != -1){
        if(opt == 'v'){
            numOfVertex=atoi(optarg);
        }
        else if(opt == 'e'){
            numOfEdges = atoi(optarg);;
        }else if(opt == 'r'){
            RandomSeed = atoi(optarg);;
        }
    }
    if(numOfEdges==-1 || numOfVertex==-1 || RandomSeed==-1){
        std::cerr<<"Usage: ./"<<argv[0]<<" -v numOfVertex (int) -e numOfEdge(int) -r Randomseed(int)"<<std::endl;
        exit(1);
    }
    unsigned long maxEdge = (numOfVertex -1)*numOfVertex/2;
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
        try{
            g.addEdge(src,dest);
            edgesAdded++;
        }catch(const std::exception& e){

        }
    }
    std::vector<int> resualt = g.findEuler();
    if(resualt.size()!=0){
        std::cout<<"The resualt is:"<<std::endl;
        for(size_t i=0;i<resualt.size();i++){
            if(i!=resualt.size()-1){
                std::cout<<resualt[i]<<" -> ";
            }
            else{
                std::cout<<resualt[i]<<std::endl;
            }
        }
    }
}

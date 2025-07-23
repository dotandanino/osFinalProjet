CXX=clang++
CXXFLAGS=-std=c++2a -g -Wall -Wextra
CXXFLAGS = -O0 -Wall


SERVER = server
CLIENT = client
SRCS = server.cpp
OBJ = Graph.o algoFactory.o maxFlowAlgo.o sccAlgo.o pathCoverAlgo.o mstAlgo.o

all : $(SERVER) $(CLIENT)

$(SERVER):$(SRCS) $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(SRCS) -o $(SERVER)

$(CLIENT):client.cpp 
	$(CXX) $(CXXFLAGS) client.cpp -o $(CLIENT)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER) $(CLIENT) $(OBJ)
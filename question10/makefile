CXX=clang++
CXXFLAGS=-std=c++2a -g -Wall -Wextra
CXXFLAGS = -O0 -Wall


SERVER = server
CLIENT = client
SRCS = ../question9/server.cpp
OBJ = ../question7/Graph.o ../question7/algoFactory.o ../question7/maxFlowAlgo.o ../question7/sccAlgo.o ../question7/pathCoverAlgo.o ../question7/mstAlgo.o

all : $(SERVER) $(CLIENT)

$(SERVER):$(SRCS) $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(SRCS) -o $(SERVER)

$(CLIENT):../question9/client.cpp 
	$(CXX) $(CXXFLAGS) ../question9/client.cpp -o $(CLIENT)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

valgrind_server: $(SERVER) $(CLIENT)
	valgrind --leak-check=full --track-origins=yes ./$(SERVER)

valgrind_client: $(CLIENT)
	valgrind --leak-check=full --track-origins=yes ./$(CLIENT)

helgrind_server: $(SERVER) $(CLIENT)
	valgrind --tool=helgrind ./$(SERVER)



clean:
	rm -f $(SERVER) $(CLIENT) $(OBJ)
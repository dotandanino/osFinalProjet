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
	valgrind --leak-check=full --track-origins=yes --log-file=server_valgrind_report.txt ./$(SERVER)

valgrind_client: $(CLIENT)
	valgrind --leak-check=full --track-origins=yes --log-file=client_valgrind_report.txt ./$(CLIENT)

helgrind_server: $(SERVER)
	valgrind --tool=helgrind --log-file=server_helgrind_report.txt ./$(SERVER)

helgrind_client: $(CLIENT)
	valgrind --tool=helgrind --log-file=client_helgrind_report.txt ./$(CLIENT)

callgrind_server: $(SERVER)
	valgrind --tool=callgrind --callgrind-out-file=callgrind_server.out ./$(SERVER)

callgrind_client: $(CLIENT)
	valgrind --tool=callgrind --callgrind-out-file=callgrind_client.out ./$(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT) $(OBJ)
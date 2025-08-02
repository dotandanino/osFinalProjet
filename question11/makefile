CXX=g++
CXXFLAGS_SC = -std=c++2a -g -Wall --coverage
CXXFLAGS=-std=c++2a -g -Wall -Wextra -O0

COVERAGE_FOLDER = coverage_folder
SERVER = server
CLIENT = client
OBJ = ../question7/Graph.o ../question7/algoFactory.o ../question7/maxFlowAlgo.o ../question7/sccAlgo.o ../question7/pathCoverAlgo.o ../question7/mstAlgo.o

all : $(SERVER) $(CLIENT)

$(SERVER):server.cpp $(OBJ)
	$(CXX) $(CXXFLAGS_SC) $(OBJ) server.cpp -o $(SERVER)

$(CLIENT):client.cpp 
	$(CXX) $(CXXFLAGS_SC) client.cpp -o $(CLIENT)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_SC) -c $< -o $@

coverage: $(SERVER) $(CLIENT)
	- gcov server.cpp server-server.gcda server-server.gcno
	- gcov client.cpp client-client.gcda client-client.gcno
	- gcov ../question7/Graph.cpp ../question7/algoFactory.cpp ../question7/maxFlowAlgo.cpp ../question7/sccAlgo.cpp ../question7/pathCoverAlgo.cpp ../question7/mstAlgo.cpp
	- rm -f *.h.gcda *.h.gcno *.h.gcov

clean:
	rm -f $(SERVER) $(CLIENT) $(OBJ) *.gcda *.gcno *.gcov ../question7/*.gcda ../question7/*.gcno ../question7/*.gcov
.PHONY: all clean coverage
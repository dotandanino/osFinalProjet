CXX=g++
CXXFLAGS_SC = -std=c++2a -g -Wall --coverage
CXXFLAGS=-std=c++2a -g -Wall -Wextra -O0

COVERAGE_FOLDER = coverage_folder
SERVER = server
CLIENT = client
SRCS = server.cpp
OBJ = ../question7/Graph.o ../question7/algoFactory.o ../question7/maxFlowAlgo.o ../question7/sccAlgo.o ../question7/pathCoverAlgo.o ../question7/mstAlgo.o

all : $(SERVER) $(CLIENT)

$(SERVER):$(SRCS) $(OBJ)
	$(CXX) $(CXXFLAGS_SC) $(OBJ) $(SRCS) -o $(SERVER)

$(CLIENT):client.cpp 
	$(CXX) $(CXXFLAGS_SC) client.cpp -o $(CLIENT)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_SC) -c $< -o $@

coverage: $(SERVER) $(CLIENT)
	- gcov server.cpp server-server.gcda server-server.gcno
	- gcov client.cpp client-client.gcda client-client.gcno
	- rm -f *.h.gcda *.h.gcno *.h.gcov
# - mkdir -p $(COVERAGE_FOLDER)
# - mv *.gcda *.gcno *.gcov $(COVERAGE_FOLDER)

clean:
	rm -f $(SERVER) $(CLIENT) $(OBJ) *.gcda *.gcno *.gcov ../question7/*.gcda ../question7/*.gcno ../question7/*.gcov
	rm -rf $(COVERAGE_FOLDER)
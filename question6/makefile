CXX=clang++
CXXFLAGS=-std=c++2a -g -Wall -Wextra
CXXFLAGS = -O0 -Wall


SERVER = server
CLIENT = client
SRCS = server.cpp
OBJ = ../question2/Graph.o

all : $(SERVER) $(CLIENT)

$(SERVER):$(SRCS) $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(SRCS) -o $(SERVER)

$(CLIENT):client.cpp 
	$(CXX) $(CXXFLAGS) client.cpp -o $(CLIENT)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER) $(CLIENT) $(OBJ)
CXX=clang++
CXXFLAGS=-std=c++2a -g -O0 -Wall -Wextra

TARGET = randomGraph
HEADERS = ../question2/Graph.hpp
OBJ = randomGraph.o ../question2/Graph.o
all:$(TARGET)

$(TARGET) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ) 

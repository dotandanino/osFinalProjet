CXX=g++
CXXFLAGS=-std=c++2a -g -Wall -Wextra -c
LDFLAGS = -fprofile-arcs -ftest-coverage

TARGET = randomGraph
TARGET2 = randomGraph2
OBJ2 = randomGraph2.o Graph2.o

HEADERS = ../question2/Graph.hpp
OBJ = randomGraph.o Graph.o
all:$(TARGET) profiling valgrind valgrindCallGraph

$(TARGET) : $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -pg -o $(TARGET) -O2

randomGraph.o: ../question3/randomGraph.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -pg -c $< -o $@
	
Graph.o: ../question2/Graph.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

coverage: $(TARGET)
	-echo | ./$(TARGET) -v 5 -e 20 -r 80
	-echo | ./$(TARGET) -v 5 -e 20
	-echo | ./$(TARGET) -v 5 -e 10 -r 80
	-echo | ./$(TARGET) -v 5 -e 8 -r 80
	gcov randomGraph.cpp
	rm -f *.h.gcov Graph.gcda Graph.gcno

valgrind: $(TARGET)
	-valgrind --leak-check=full --track-origins=yes ./$(TARGET) -v 5 -e 20 -r 80 > valgrind.txt
	-valgrind --leak-check=full --track-origins=yes ./$(TARGET) -v 5 -e 20 >> valgrind.txt
	-valgrind --leak-check=full --track-origins=yes ./$(TARGET) -v 5 -e 10 -r 80 >> valgrind.txt
	-valgrind --leak-check=full --track-origins=yes ./$(TARGET) -v 5 -e 8 -r 80 >> valgrind.txt

valgrindCallGraph: $(TARGET2)
	-valgrind --tool=callgrind --callgrind-out-file=callgrind1.out ./$(TARGET2) -v 100 -e 4950 -r 42
	-valgrind --tool=callgrind --callgrind-out-file=callgrind2.out ./$(TARGET2) -v 5 -e 20
	-valgrind --tool=callgrind --callgrind-out-file=callgrind3.out ./$(TARGET2) -v 1000 -e 49000 -r 42
	callgrind_annotate callgrind1.out > callgraph_report1.txt
	callgrind_annotate callgrind2.out > callgraph_report2.txt
	callgrind_annotate callgrind3.out > callgraph_report3.txt

$(TARGET2): $(OBJ2)
	$(CXX) $(OBJ2) $(LDFLAGS) -O0 -o $(TARGET2)

randomGraph2.o: ../question3/randomGraph.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

Graph2.o: ../question2/Graph.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

profiling:clean $(TARGET)
	./$(TARGET) -v 524 -e 1058 -r 42 
	gprof --brief  -b -p -q randomGraph gmon.out > profiling.txt

clean:
	rm -f $(TARGET) $(OBJ) *.gcda *.h.gcov *.gcno *.out $(TARGET2)
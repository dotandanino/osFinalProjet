CXX=g++
CXXFLAGS=-std=c++2a -g -Wall -Wextra -c
LDFLAGS = -fprofile-arcs -ftest-coverage

TARGET = randomGraph
TARGET2 = randomGraph2
OBJ2 = randomGraph2.o Graph2.o

HEADERS = ../question2/Graph.hpp
OBJ = randomGraph.o Graph.o
all:$(TARGET) $(TARGET2) profiling valgrind valgrindCallGraph

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -O0 -o $(TARGET)

$(TARGET2) : $(OBJ2)
	$(CXX) $(OBJ2) -pg -o $(TARGET2) -O2

randomGraph.o: ../question3/randomGraph.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -pg -c $< -o $@
	
Graph.o: Graph.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

coverage: $(TARGET)
	-echo | ./$(TARGET) -v 5 -e 20 -r 80
	-echo | ./$(TARGET) -v 5 -e 20
	-echo | ./$(TARGET) -v 5 -e 10 -r 80
	-echo | ./$(TARGET) -v 5 -e 8 -r 80
	-echo | ./$(TARGET) -v 5 -e 2 -r 80
	gcov randomGraph.cpp
	gcov Graph.cpp
	rm -f *.h.gcov

valgrind: $(TARGET)
	-valgrind --tool=memcheck --leak-check=full --track-origins=yes \
		--log-file=valgrind_report1.txt ./$(TARGET) -v 5 -e 10 -r 42
	-valgrind --tool=memcheck --leak-check=full --track-origins=yes \
		--log-file=valgrind_report2.txt ./$(TARGET) -v 5 -e 20 -r 42
	-valgrind --tool=memcheck --leak-check=full --track-origins=yes \
		--log-file=valgrind_report3.txt ./$(TARGET) -v 5 -e 20

valgrindCallGraph: $(TARGET)
	-valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./$(TARGET) -v 5 -e 10 -r 42
	-valgrind --tool=callgrind --callgrind-out-file=callgrind2.out ./$(TARGET) -v 100 -e 2250 -r 42

randomGraph2.o: ../question3/randomGraph.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -pg -c $< -o $@

Graph2.o: Graph.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -pg -c $< -o $@

profiling:$(TARGET2)
	./$(TARGET2) -v 524 -e 1058 -r 42 
	gprof --brief  -b -p -q randomGraph2 gmon.out > profiling.txt

.PHONY: all coverage profiling valgrind valgrindCallGraph clean

clean:
	rm -f $(TARGET) $(OBJ) *.gcda *.h.gcov *.gcno *.out $(TARGET2) $(OBJ2)
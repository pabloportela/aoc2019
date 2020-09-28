CXX = g++
CXXFLAGS = -Wall -v -Iinclude

# dirs
SOURCE_DIR = src
BUILD_DIR = src
INCLUDE = include
BIN = bin
TESTS = tests


all: day4_2 day4_2_test


# test
day4_2_test: $(TESTS)/day4_2_test.cpp tests.o day4_2_lib.o
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

# this one includes Catch2
tests.o: $(TESTS)/tests.cpp
	$(CXX) $(CXXFLAGS) -c $^

# app
day4_2: $(SOURCE_DIR)/day4_2.cpp day4_2_lib.o
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

# library
day4_2_lib.o: $(SOURCE_DIR)/day4_2_lib.cpp
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -rf *.o

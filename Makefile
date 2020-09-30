CXX = g++
CXXFLAGS = -Wall -v -Iinclude -std=c++11

# dirs
SOURCE_DIR = src
BUILD_DIR = src
TEST_DIR = tests
BUILD_DIR = build



all: output_dirs day4_2 day4_2_test

# test
day4_2_test: $(TEST_DIR)/day4_2_test.cpp $(BUILD_DIR)/tests.o $(BUILD_DIR)/day4_2_lib.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# this one includes Catch2
$(BUILD_DIR)/tests.o: $(TEST_DIR)/tests.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

# app
day4_2: $(SOURCE_DIR)/day4_2.cpp $(BUILD_DIR)/day4_2_lib.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# library
$(BUILD_DIR)/day4_2_lib.o: $(SOURCE_DIR)/day4_2_lib.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

output_dirs:
	mkdir -p $(BUILD_DIR)

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)

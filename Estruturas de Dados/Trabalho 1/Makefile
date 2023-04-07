CXX = g++
CXXFLAGS = -Wall -Wextra

SRC_DIR = .
OBJ_DIR = objects

ifeq ($(shell uname), Linux)
	EXE = main
	TEST_EXE = test
else
	EXE = main.exe
	TEST_EXE = test.exe
endif

$(EXE): utils.o main.cpp Profiler.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: utils.o test.o Profiler.o
	$(CXX) $(CXXFLAGS) lists_tests.cpp $^ -o $(TEST_EXE)

clean:
		rm -rf $(EXE) $(OBJ_DIR)

.PHONY: clean

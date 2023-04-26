CXX = g++
CXXFLAGS = -Wall -Wextra
CXXDEBUG_FLAGS = -Wall -Wextra -g3 -ggdb

SRC_DIR = .
OBJ_DIR = objects

ifeq ($(shell uname), Linux)
	EXE = main
	DEBUG_EXE = main.debug
	TEST_EXE = test
	TEST_DEBUG_EXE = test.debug
else
	EXE = main.exe
	DEBUG_EXE = main.debug.exe
	TEST_EXE = test.exe
	TEST_DEBUG_EXE = test.debug.exe
endif

$(EXE): utils.o main.cpp Profiler.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ 

debug: utils.o main.cpp Profiler.cpp
	$(CXX) $(CXXDEBUG_FLAGS) $^ -o $(DEBUG_EXE) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: utils.o test.o Profiler.o
	$(CXX) $(CXXFLAGS) lists_tests.cpp $^ -o $(TEST_EXE)

debug-test: utils.o test.o Profiler.o
	$(CXX) $(CXXDEBUG_FLAGS) lists_tests.cpp $^ -o $(TEST_DEBUG_EXE)

clean:
		rm -rf $(EXE) $(OBJ_DIR)

.PHONY: clean

CXX = g++
CXXFLAGS = -Wall -Wextra

SRC_DIR = .

ifeq ($(shell uname), Linux)
	EXE = main
	DEBUG_EXE = main.debug
	TEST_EXE = test
	TEST_DEBUG_EXE = test.debug
	OBJ_DIR = objects/linux
	CXXDEBUG_FLAGS = -Wall -Wextra -g3 -ggdb -fsanitize=address
	CXXDEBUG_VALGRIND_FLAGS = -Wall -Wextra -g3 -ggdb
else
	EXE = main.exe
	DEBUG_EXE = main.debug.exe
	TEST_EXE = test.exe
	TEST_DEBUG_EXE = test.debug.exe
	OBJ_DIR = objects/windows
	CXXDEBUG_FLAGS = -Wall -Wextra -g3 -ggdb
endif

$(EXE): $(OBJ_DIR)/utils.o $(OBJ_DIR)/main.o $(OBJ_DIR)/Profiler.o
	$(CXX) $(CXXFLAGS) $^ -o $@ 

debug: $(OBJ_DIR)/utils.o $(OBJ_DIR)/main.o $(OBJ_DIR)/Profiler.o
	$(CXX) $(CXXDEBUG_FLAGS) $^ -o $(DEBUG_EXE) 

ifeq ($(shell uname), Linux)
debug-val: $(OBJ_DIR)/utils.o $(OBJ_DIR)/main.o $(OBJ_DIR)/Profiler.o
	$(CXX) $(CXXDEBUG_VALGRIND_FLAGS) $^ -o $(DEBUG_EXE) 
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

test: $(OBJ_DIR)/utils.o $(OBJ_DIR)/test.o $(OBJ_DIR)/Profiler.o
	$(CXX) $(CXXFLAGS) lists_tests.cpp $^ -o $(TEST_EXE)

debug-test: $(OBJ_DIR)/utils.o $(OBJ_DIR)/test.o $(OBJ_DIR)/Profiler.o
	$(CXX) $(CXXDEBUG_FLAGS) lists_tests.cpp $^ -o $(TEST_DEBUG_EXE)

ifeq ($(shell uname), Linux)
debug-test-val: $(OBJ_DIR)/utils.o $(OBJ_DIR)/test.o $(OBJ_DIR)/Profiler.o
	$(CXX) $(CXXDEBUG_VALGRIND_FLAGS) lists_tests.cpp $^ -o $(TEST_DEBUG_EXE)
endif

clean:
		rm -rf $(EXE) $(OBJ_DIR)

.PHONY: clean
	

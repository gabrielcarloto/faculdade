CXX = g++
CXXFLAGS = -Wall -Wextra
CXXBUILDFLAGS = $(CXXFLAGS) -O3
CXX_BASE_DEBUG_FLAGS = -g3 -ggdb

SRC_DIR = .

BASE_EXE_NAME = main
BASE_DEBUG_EXT = debug
BASE_TEST_EXE_NAME = test
BASE_OBJECTS_DIR = objects

ifeq ($(shell uname), Linux)
	EXE = $(BASE_EXE_NAME)
	DEBUG_EXE = $(BASE_EXE_NAME).$(BASE_DEBUG_EXT)
	TEST_EXE = $(BASE_TEST_EXE_NAME)
	TEST_DEBUG_EXE = $(BASE_TEST_EXE_NAME).$(BASE_DEBUG_EXT)
	OBJ_DIR = $(BASE_OBJECTS_DIR)/linux
	CXXDEBUG_FLAGS = $(CXX_FLAGS) $(CXX_BASE_DEBUG_FLAGS) -fsanitize=address
	CXXDEBUG_VALGRIND_FLAGS = $(CXX_FLAGS) $(CXX_BASE_DEBUG_FLAGS)
else
	EXE = $(BASE_EXE_NAME).exe
	DEBUG_EXE = $(BASE_EXE_NAME).$(BASE_DEBUG_EXT).exe
	TEST_EXE = $(BASE_TEST_EXE_NAME).exe
	TEST_DEBUG_EXE = $(BASE_TEST_EXE_NAME).$(BASE_DEBUG_EXT).exe
	OBJ_DIR = $(BASE_OBJECTS_DIR)/windows
	CXXDEBUG_FLAGS = $(CXX_FLAGS) $(CXX_BASE_DEBUG_FLAGS)
endif

HEADERS = BaseList.h Vector.h List.h BasicLinkedList.h
OBJECTS = $(OBJ_DIR)/utils.o $(OBJ_DIR)/main.o $(OBJ_DIR)/Profiler.o
TEST_OBJECTS = $(OBJ_DIR)/utils.o $(OBJ_DIR)/test.o $(OBJ_DIR)/Profiler.o

$(EXE): $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ 

build: $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXBUILDFLAGS) $(OBJECTS) -o $(EXE)

debug: $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXDEBUG_FLAGS) $(OBJECTS) -o $(DEBUG_EXE) 

ifeq ($(shell uname), Linux)
debug-val: $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXDEBUG_VALGRIND_FLAGS) $(OBJECTS) -o $(DEBUG_EXE) 
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

test: $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CXXFLAGS) lists_tests.cpp $(TEST_OBJECTS) -o $(TEST_EXE)

debug-test: $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CXXDEBUG_FLAGS) lists_tests.cpp $(TEST_OBJECTS) -o $(TEST_DEBUG_EXE)

ifeq ($(shell uname), Linux)
debug-test-val: $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CXXDEBUG_VALGRIND_FLAGS) lists_tests.cpp $(TEST_OBJECTS) -o $(TEST_DEBUG_EXE)
endif

clean:
		rm -rf $(EXE) $(OBJ_DIR)

.PHONY: clean
	

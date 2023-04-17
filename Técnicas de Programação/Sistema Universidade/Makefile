# Define the compiler and compiler flags
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror 

# Define the name of the executable
EXE = main.exe

# Define the directories for source files and object files
SRC_DIR = .
OBJ_DIR = obj

# Define the source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Define the rule to build the executable
$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Define the rule to build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Define the clean rule
clean:
	rm -rf $(EXE) $(OBJ_DIR)

.PHONY: clean


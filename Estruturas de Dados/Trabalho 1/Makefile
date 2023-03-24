CXX = g++
CXXFLAGS = -Wall -Wextra

SRC_DIR = .
OBJ_DIR = objects

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

EXE = main.exe

$(EXE): $(OBJS)
	$(CXX) $(CFLAGS) $^ -o $@ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
		rm -Force -Recurse $(EXE) $(OBJ_DIR)

.PHONY: clean

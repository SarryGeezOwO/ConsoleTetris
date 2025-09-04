CXX := g++
CXXFLAGS := -std=c++11 -Wall

SRC_DIR := app/src
OBJ_DIR := out/build
BIN_DIR := out/bin

APP_NAME := TetrisConsole
EXE 	 := $(BIN_DIR)/$(APP_NAME).exe

# Add all src subfolder below:
SRC_DIRS := $(SRC_DIR)
SRC 	 := $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
OBJ 	 := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(subst $(SRC_DIR)/,,$(SRC)))

.PHONY: clean run create_dirs

all: $(EXE)

# Also update this when SRC_DIRS is modified
SRC_CREATE_DIRS :=
create_dirs:
	for dir in $(foreach source, $(SRC_CREATE_DIRS), $(OBJ_DIR)/$(source)); do \
		mkdir dir \
	done

$(EXE) : $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@echo ---------------- $(APP_NAME) ----------------
	@$(EXE)

# Deletes .exe and Object files
clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*/*.o $(EXE)
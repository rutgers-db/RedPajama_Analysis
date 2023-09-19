SRC_DIR := src
BIN_DIR = bin
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

LDFLAGS := -O3 -std=c++14 -fopenmp
CPPFLAGS := -O3 -std=c++14  -fopenmp

all: main

main: $(OBJ_DIR)/%.o
	g++ $(LDFLAGS)  -o $(BIN_DIR)/$@.exe $<
	
$(OBJ_DIR)/%.o: $(SRC_FILES)
	g++ $(CPPFLAGS) -c -o $@ $<

sample: $(OBJ_DIR)/sample.o
	g++ $(LDFLAGS)  -o $(BIN_DIR)/$@.exe $<
	
$(OBJ_DIR)/sample.o: $(SRC_DIR)/sample_data.cpp
	g++ $(CPPFLAGS) -c -o $@ $<

.PHONY :clean
clean:
	rm ./obj/*.o *.o 

COMPILER = g++-9
SRC_DIR := ./src
SRC_FILES := $(wildcard $(SRC_DIR)/*.hpp)

CPPFLAGS = -O3 -fopenmp -std=c++17 -ltbb
all: main

main: main.cpp $(SRC_FILES)
	${COMPILER} ${CPPFLAGS} -o ./bin/$@ $<

merge: merge_simp.cpp $(SRC_FILES)
	${COMPILER} ${CPPFLAGS} -o ./bin/$@ $<

sort: sort_each.cpp $(SRC_FILES)
	${COMPILER} ${CPPFLAGS} -o ./bin/$@ $<


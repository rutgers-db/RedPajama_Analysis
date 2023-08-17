CXX = g++

# Main Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin
TEST_DIR = test

# Sub Directories
UTIL_DIR = $(SRCDIR)/util
SETJOIN_DIR = $(SRCDIR)/set_join

# Compiler flags
CXXFLAGS = -std=c++17 -O3 -fopenmp -g

# Files and folders
SRCS = $(wildcard $(SRCDIR)/*.cc) $(wildcard $(SRCDIR)/set_join/*.cc) $(wildcard $(UTIL_DIR)/*.cc)
OBJS = $(SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o) 

UTIL_SRCS = $(wildcard $(UTIL_DIR)/*.cc)
UTIL_OBJS = $(UTIL_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)
SETJOIN_SRCS = $(wildcard $(SETJOIN_DIR)/*.cc)
SETJOIN_OBJS = $(SETJOIN_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

TEST_SRC_FILES = $(wildcard $(TEST_DIR)/*.cc)
TEST_BIN_FILES = $(patsubst $(TEST_DIR)/%.cc, $(TEST_DIR)/%, $(TEST_SRC_FILES))

# Target file
TARGET = $(BINDIR)/main.exe

# Build rules
all: $(TARGET)

$(TARGET): $(OBJDIR)/main.o $(UTIL_OBJS) $(SETJOIN_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_BIN_FILES)
	# @for test in $(TEST_BIN_FILES); do \
	# 	./$$test; \
	# done

$(TEST_DIR)/%: $(TEST_DIR)/%.cc $(UTIL_OBJS)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o $@ $(UTIL_OBJS)

sort: $(OBJDIR)/sort_simp.o
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o ./bin/$@.exe $(UTIL_OBJS)

$(OBJDIR)/sort_simp.o: $(UTIL_OBJS) src/sort_simp.cc
	$(CXX) $(CXXFLAGS) -c src/sort_simp.cc -o $@

sketch_join: $(OBJDIR)/sketch_join.o $(UTIL_OBJS) $(SETJOIN_OBJS)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o ./bin/$@.exe $(UTIL_OBJS) $(SETJOIN_OBJS)

$(OBJDIR)/sketch_join.o: $(UTIL_OBJS) src/sketch_join.cc
	$(CXX) $(CXXFLAGS) -c src/sketch_join.cc -o $@

sort_bottomks: $(OBJDIR)/sort_bottomk.o
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o ./bin/$@.exe $(UTIL_OBJS)

$(OBJDIR)/sort_bottomk.o: $(UTIL_OBJS) src/sort_bottomk.cc
	$(CXX) $(CXXFLAGS) -c src/sort_bottomk.cc -o $@

extract: $(OBJDIR)/extractSet.o
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o ./bin/$@.exe $(UTIL_OBJS)

$(OBJDIR)/extractSet.o: $(UTIL_OBJS) src/extractSet.cc
	$(CXX) $(CXXFLAGS) -c src/extractSet.cc -o $@

compare: $(OBJDIR)/compare.o
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o ./bin/$@.exe $(UTIL_OBJS)

$(OBJDIR)/compare.o: $(UTIL_OBJS) src/compare.cc
	$(CXX) $(CXXFLAGS) -c src/compare.cc -o $@

verify: $(OBJDIR)/verify.o
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o ./bin/$@.exe $(UTIL_OBJS)

$(OBJDIR)/verify.o: $(UTIL_OBJS) src/verify.cc
	$(CXX) $(CXXFLAGS) -c src/verify.cc -o $@

clean:
	@rm -r obj
	@rm ./bin/.nfs*
	

clean_test:
	@for test in $(TEST_BIN_FILES); do \
		rm -f ./$$test; \
		echo $$test "removed";\
	done
	

.PHONY: clean clean_test
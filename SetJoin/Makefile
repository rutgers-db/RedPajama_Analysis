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

extract: $(OBJDIR)/extractSet.o
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o $@ $(UTIL_OBJS)

$(OBJDIR)/extractSet.o: $(UTIL_OBJS)
	$(CXX) $(CXXFLAGS) -c src/extractSet.cc -o $@

clean:
	@rm -r obj
	@rm ./bin/.nfs*
	

clean_test:
	@for test in $(TEST_BIN_FILES); do \
		rm -f ./$$test; \
		echo $$test "removed";\
	done
	

.PHONY: clean clean_test
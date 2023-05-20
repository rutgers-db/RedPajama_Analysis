CXX = g++

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin
TEST_DIR = test
UTIL_DIR = $(SRCDIR)/util

# Compiler flags
CXXFLAGS = -std=c++17 -O3 -g -fopenmp -pg

# Files and folders
SRCS = $(wildcard $(SRCDIR)/*.cc) $(wildcard $(SRCDIR)/overlap_join/*.cc) $(wildcard $(UTIL_DIR)/*.cc)
OBJS = $(SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

TEST_SRC_FILES = $(wildcard $(TEST_DIR)/*.cc)
TEST_BIN_FILES = $(patsubst $(TEST_DIR)/%.cc, $(TEST_DIR)/%, $(TEST_SRC_FILES))

# Target file
TARGET = $(BINDIR)/main.exe

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_BIN_FILES)
	@for test in $(TEST_BIN_FILES); do \
		./$$test; \
	done

$(TEST_DIR)/%: $(TEST_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o $@

clean:
	$(RM) -r $(OBJDIR) $(TARGET)

clean_test:
	@for test in $(TEST_BIN_FILES); do \
		rm -f ./$$test; \
		echo $$test "removed";\
	done

.PHONY: clean clean_test
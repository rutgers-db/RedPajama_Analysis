CXX = g++

# Main Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin
TEST_DIR = test

# Sub Directories
UTIL_DIR = $(SRCDIR)/util
OVLPJOIN_DIR = $(SRCDIR)/overlap_rsjoin

# Compiler flags
CXXFLAGS = -std=c++17 -O3 -g -fopenmp -pg

# Files and folders
SRCS = $(wildcard $(SRCDIR)/*.cc) $(wildcard $(SRCDIR)/overlap_rsjoin/*.cc) $(wildcard $(UTIL_DIR)/*.cc)
OBJS = $(SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

UTIL_SRCS = $(wildcard $(UTIL_DIR)/*.cc)
UTIL_OBJS = $(UTIL_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)
OVLPJOIN_SRCS = $(wildcard $(OVLPJOIN_DIR)/*.cc)
OVLPJOIN_OBJS = $(OVLPJOIN_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

TEST_SRC_FILES = $(wildcard $(TEST_DIR)/*.cc)
TEST_BIN_FILES = $(patsubst $(TEST_DIR)/%.cc, $(TEST_DIR)/%, $(TEST_SRC_FILES))

# Target file
TARGET = $(BINDIR)/main.exe

# Build rules
all: $(TARGET)

$(TARGET): $(OBJDIR)/main.o $(UTIL_OBJS) $(OVLPJOIN_OBJS)
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

clean:
	@rm ./bin/.nfs*
	$(RM) -r $(OBJDIR) $(TARGET)

clean_test:
	@for test in $(TEST_BIN_FILES); do \
		rm -f ./$$test; \
		echo $$test "removed";\
	done

# build Rule for read_simPair
read: $(OBJDIR)/read_simPair.o
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o $@ $(UTIL_OBJS)
$(OBJDIR)/read_simPair.o: $(UTIL_OBJS)
	$(CXX) $(CXXFLAGS) -c src/read_simPair.cc -o $@

merge: $(OBJDIR)/cal_simPair.o
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o $@ $(UTIL_OBJS)
$(OBJDIR)/cal_simPair.o: $(UTIL_OBJS) src/cal_simPair.cc
	$(CXX) $(CXXFLAGS) -c src/cal_simPair.cc -o $@
	
.PHONY: clean clean_test
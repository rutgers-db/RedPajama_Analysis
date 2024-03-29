CXX = g++

# Main Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Sub Directories
UTIL_DIR = $(SRCDIR)/util

# Compiler flags
CXXFLAGS = -std=c++17 -O3 -fopenmp -g

# Files and folders
SRCS = $(wildcard $(SRCDIR)/*.cc)  $(wildcard $(UTIL_DIR)/*.cc)
OBJS = $(SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o) 

UTIL_SRCS = $(wildcard $(UTIL_DIR)/*.cc)
UTIL_OBJS = $(UTIL_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

# Target file
TARGET = $(BINDIR)/main.exe

# Build rules
all: $(TARGET)

$(TARGET): $(OBJDIR)/main.o $(UTIL_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

cmp_simpairs: $(OBJDIR)/cmp_simpairs.o $(UTIL_OBJS) 
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $< -o ./bin/$@.exe $(UTIL_OBJS)

$(OBJDIR)/cmp_simpairs.o: $(UTIL_OBJS) src/cmp_simpairs.cc
	$(CXX) $(CXXFLAGS) -c src/cmp_simpairs.cc -o $@

clean:
	@rm -r obj
	@rm ./bin/.nfs*

.PHONY: clean 
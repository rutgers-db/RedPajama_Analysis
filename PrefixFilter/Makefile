CXX = g++

# Main Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Sub Directories
UTIL_DIR = $(SRCDIR)/util
PF_DIR = $(SRCDIR)/prefixfilter

# Compiler flags
CXXFLAGS = -std=c++17 -O3 -fopenmp -g

# Files and folders
SRCS = $(wildcard $(SRCDIR)/*.cc) $(wildcard $(SRCDIR)/prefixfilter/*.cc) $(wildcard $(UTIL_DIR)/*.cc)
OBJS = $(SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o) 

UTIL_SRCS = $(wildcard $(UTIL_DIR)/*.cc)
UTIL_OBJS = $(UTIL_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)
PF_SRCS = $(wildcard $(PF_DIR)/*.cc)
PF_OBJS = $(PF_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

# Target file
TARGET = $(BINDIR)/main.exe

# Build rules
all: $(TARGET)

$(TARGET): $(OBJDIR)/main.o $(UTIL_OBJS) $(PF_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	@rm -r obj
	@rm ./bin/.nfs*
	

.PHONY: clean
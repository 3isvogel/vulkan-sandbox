#   This script is an improvement of:
# Thomas Daley
# September 13, 2021
# For the original version follow:
# https://gist.github.com/tomdaley92/190c68e8a84038cc91a5459409e007df

# A generic build template for C/C++ programs

# executable name
EXE = app

# C compiler
CC = gcc
# C++ compiler
CXX = g++
# linker
LD = g++

# C flags
CFLAGS =
# C++ flags
CXXFLAGS = -std=c++17
# C/C++ flags
CPPFLAGS = -O2
# dependency-generation flags
DEPFLAGS = -MMD -MP -Isrc/
# linker flags
LDFLAGS =
# library flags
LDLIBS =
# packages used
PKG = vulkan glfw3 glm
# build directories
BIN = bin
OBJ = obj
SRC = src
SHA = shaders
# Additional dependencies on which rebuild objects
# useful when changing stuff in the makefile to rebuild the project with new options
OBJDEPS = Makefile 

################################################################################
# Ideally you should not go under this line
################################################################################

CFLAGS+=$(shell pkg-config --cflags $(PKG))
CXXFLAGS+=$(shell pkg-config --cflags $(PKG))
LDFLAGS+=$(shell pkg-config --libs $(PKG))

SOURCES := $(wildcard $(SRC)/*.c $(SRC)/**/*.c $(SRC)/*.cc $(SRC)/**/*.cc $(SRC)/*.cpp $(SRC)/**/*.cpp $(SRC)/*.cxx $(SRC)/**/*.cxx)

OBJECTS := \
	$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(shell find $(SRC) -name "*.c")) \
	$(patsubst $(SRC)/%.cc, $(OBJ)/%.o, $(shell find $(SRC) -name "*.cc")) \
	$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(shell find $(SRC) -name "*.cpp")) \
	$(patsubst $(SRC)/%.cxx, $(OBJ)/%.o, $(shell find $(SRC) -name "*.cxx"))

SHADERS := \
	$(patsubst $(SHA)/%.frag, $(SHA)/%Frag.spv, $(wildcard $(SHA)/*.frag)) \
	$(patsubst $(SHA)/%.vert, $(SHA)/%Vert.spv, $(wildcard $(SHA)/*.vert))

# include compiler-generated dependency rules
DEPENDS := $(OBJECTS:.o=.d)

# compile C source
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# compile C++ source
COMPILE.cxx = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
# link objects
LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $@
# shaders creation
COMPILE.spv = glslc -o $@

ENSURE = @mkdir -p $(dir $@) 2> /dev/null || true

.DEFAULT_GOAL = all

.PHONY: all
all: $(BIN)/$(EXE) $(SHADERS)

$(BIN)/$(EXE): $(SRC) $(OBJ) $(BIN) $(OBJECTS)
	$(LINK.o)

$(SRC):
	mkdir -p $(SRC)

$(OBJ):
	mkdir -p $(OBJ)

$(BIN):
	mkdir -p $(BIN)

$(SHA):
	mkdir -p $(SHA)

$(OBJ)/%.o:	$(SRC)/%.c $(OBJDEPS)
	$(ENSURE)
	$(COMPILE.c) $<

$(OBJ)/%.o:	$(SRC)/%.cc $(OBJDEPS)
	$(ENSURE)
	$(COMPILE.cxx) $<

$(OBJ)/%.o:	$(SRC)/%.cpp $(OBJDEPS)
	$(ENSURE)
	$(COMPILE.cxx) $<

$(OBJ)/%.o:	$(SRC)/%.cxx $(OBJDEPS)
	$(ENSURE)
	$(COMPILE.cxx) $<

$(SHA)/%Frag.spv: $(SHA)/%.frag
	$(COMPILE.spv) $<

$(SHA)/%Vert.spv: $(SHA)/%.vert
	$(COMPILE.spv) $<

# force rebuild
.PHONY: remake
remake:	clean $(BIN)/$(EXE)

# execute the program
.PHONY: run
run: $(BIN)/$(EXE) $(SHADERS)
	./$(BIN)/$(EXE)

# remove previous build and objects
.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDS)
	$(RM) $(SHA)/*.spv
	$(RM) $(BIN)/$(EXE)

# remove everything except source
.PHONY: reset
reset:
	$(RM) -r $(OBJ)
	$(RM) -r $(BIN)
	$(RM) $(SHA)/*.spv

# compile shaders only
.PHONY: shaders
shaders: $(SHADERS)

# clean shaders only
.PHONY: clean-shaders
clean-shaders:
	$(RM) $(SHA)/*.spv

-include $(DEPENDS)
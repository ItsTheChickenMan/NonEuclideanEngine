# makefile for non euclidean engine

# compiler info
CC=g++

# project info
OUT=NonEuclideanEngine

# directories
INSTALL_DIR=./bin/
INCLUDE_DIR=./include/
LIB_DIRS=./lib/
SRC_DIR=./src/
OBJ_DIR=$(INSTALL_DIR)obj/

# commands
ifndef ENV
	ENV=make
endif

formatslashes=$(1)
mkdir=mkdir -p $(1)
rm=rm -rf $(1)

# commands for window environment
ifeq ($(ENV),mingw)
	formatslashes=$(subst /,\\,$(1))
	mkdir=if not exist $(1) mkdir $(1)
	rm=del /Q $(1)
endif

# obj formatting
_OBJ=glad.o fileio.o shader.o application.o main.o
OBJ=$(patsubst %,$(OBJ_DIR)%,$(_OBJ))

# object sources
glad.o=$(SRC_DIR)NonEuclideanEngine/glad/glad.c $(INCLUDE_DIR)glad/glad.h
fileio.o=$(SRC_DIR)NonEuclideanEngine/fileio.cpp $(INCLUDE_DIR)NonEuclideanEngine/fileio.hpp
shader.o=$(SRC_DIR)NonEuclideanEngine/shader.cpp $(INCLUDE_DIR)NonEuclideanEngine/shader.hpp
application.o=$(SRC_DIR)NonEuclideanEngine/application.cpp $(INCLUDE_DIR)NonEuclideanEngine/application.hpp
main.o=$(SRC_DIR)main.cpp

# lib directories string (-L./dir/ -L./otherdir/)
LIB=$(patsubst %,-L%,$(LIB_DIRS))

# lib includes
LIBS=-lmingw32 -lSDL2main -lSDL2 -lopengl32

# header dependencies
HEADERDEPS=$(INCLUDE_DIR)NonEuclideanEngine/misc.hpp

# compiler flags
CXXFLAGS=-Werror -g

# make all targets
.PHONY: all
all: $(INSTALL_DIR)$(OUT)

# clean o files
.PHONY: clean
clean: clearobj all
	
.PHONY: clearobj
clearobj:
	@$(call rm,$(OBJ_DIR))
	
.PHONY: wintest
wintest:
	@echo "env = $(ENV)"
	@echo "mkdir = $(call mkdir,$(INSTALL_DIR))"
	@echo "rm = $(call rm,$(OBJ_DIR))"
	@echo "objects = $(call formatslashes,$(OBJ))"
	@echo "main = $(OBJ_DIR)main.o"

# main target
$(INSTALL_DIR)$(OUT): $(OBJ) 
	@echo building $@
	@$(CXX) -o $@ $^ $(CXXFLAGS) -I$(INCLUDE_DIR) $(LIB) $(LIBS)
	@echo built $@
	
# define obj prerequisites
$(OBJ_DIR)glad.o: $(glad.o)

$(OBJ_DIR)fileio.o: $(fileio.o) $(HEADERDEPS)
$(OBJ_DIR)shader.o: $(shader.o) $(fileio.o) $(HEADERDEPS)
$(OBJ_DIR)application.o: $(application.o) $(shader.o) $(HEADERDEPS)

$(OBJ_DIR)main.o: $(main.o) $(application.o) $(HEADERDEPS)

# obj rule
$(OBJ):
	@echo building $@
	@$(call mkdir,$(call formatslashes,$(OBJ_DIR)))
	@$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(INCLUDE_DIR) $(LIB) $(LIBS)
	@echo built $@
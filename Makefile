SRC_DIR		:= src
OBJ_DIR		:= obj
LIB_DIR		:= lib
BIN_DIR		:= bin

C_SRC		:= $(wildcard $(SRC_DIR)/*.c)
CPP_SRC		:= $(wildcard $(SRC_DIR)/*.cpp)

C_OBJ		:= $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(C_SRC)))
CPP_OBJ		:= $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(CPP_SRC)))

LIB			:= libringbuffer.so
EXE			:= main

CC			:= gcc
CXX			:= g++
LD			:= g++

CPPFLAGS	:= -Iinclude/
CFLAGS		:= --std=c23 -pedantic -Wall -Werror -Wextra
CXXFLAGS	:= --std=c++23 -pedantic -Wall -Werror -Wextra -fPIC
LDFLAGS 	:= -L$(LIB_DIR) -l:$(LIB)

.PHONY: library all clean

library: $(LIB_DIR)/$(LIB)

all: library $(BIN_DIR)/$(EXE)

$(BIN_DIR)/$(EXE): $(C_OBJ) | $(LIB_DIR)/$(LIB) $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ -o $@

$(C_OBJ): $(C_SRC) | $(OBJ_DIR)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(CPP_OBJ): $(CPP_SRC) | $(OBJ_DIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(LIB_DIR)/$(LIB): $(CPP_OBJ) | $(LIB_DIR)
	$(LD) -shared $^ -o $@

$(BIN_DIR) $(LIB_DIR) $(OBJ_DIR):
	@mkdir -p $@

clean:
	@rm -f $(BIN_DIR)/* $(LIB_DIR)/* $(OBJ_DIR)/*
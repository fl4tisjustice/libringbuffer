SRC_DIR		:= src
OBJ_DIR		:= obj
LIB_DIR		:= lib
BIN_DIR		:= bin

C_SRC		:= $(wildcard $(SRC_DIR)/*.c)
CPP_SRC		:= $(wildcard $(SRC_DIR)/*.cpp)

C_OBJ		:= $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(C_SRC)))
CPP_OBJ		:= $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(CPP_SRC)))

EXE			:= main

CPPFLAGS	:= -Iinclude/

COMMON		:= -pedantic -pedantic-errors -Wpedantic -Wall -Werror -Wextra -Wabi -O3

CFLAGS		:= --std=c23 $(COMMON)
CXXFLAGS	:= --std=c++23 -fPIC -fvisibility=hidden $(COMMON)

LDFLAGS 	= -L$(LIB_DIR) -l:$(LIB)
LIB_LDFLAGS := -shared -fPIC

.PHONY: default linux windows library

default:
	@if [ -z $(MAKECMDGOALS) ]; then \
		printf "[ERROR]\tNo target specified. Please select one of: windows linux\n"; \
		exit; \
	fi;

linux: LIB					:= libringbuffer.so
linux: CC   				:= gcc
linux: CXX  				:= g++
linux: LD   				:= g++

linux:  library $(BIN_DIR)/$(EXE)

windows: LIB				:= libringbuffer.dll
windows: CC					:= x86_64-w64-mingw32-gcc
windows: CXX				:= x86_64-w64-mingw32-g++
windows: LD					:= x86_64-w64-mingw32-g++

windows: LIB_LDFLAGS += -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,-Bdynamic,--no-whole-archive

windows: library $(BIN_DIR)/$(EXE)

library: CPPFLAGS			+= -DLIBRINGBUFFER_BUILD
library: $(CPP_OBJ) | $(LIB_DIR)
	$(LD) $(LIB_LDFLAGS) $^ -o $(LIB_DIR)/$(LIB)

$(BIN_DIR)/$(EXE): $(C_OBJ) | $(LIB_DIR)/$(LIB) $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ -o $@

$(C_OBJ): $(C_SRC) | $(OBJ_DIR)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(CPP_OBJ): $(CPP_SRC) | $(OBJ_DIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $^ -o $@


$(BIN_DIR) $(LIB_DIR) $(OBJ_DIR):
	@mkdir -p $@

clean:
	@rm -f $(BIN_DIR)/* $(LIB_DIR)/* $(OBJ_DIR)/*
CC = gcc
CCXX = g++
# add -g for debugging info
COMMON_FLAGS = -Wall -g -fmax-errors=1 -Wfatal-errors -Wno-memset-transposed-args -pthread -fPIC
CCXX_FLAGS = $(COMMON_FLAGS)
CC_FLAGS = $(COMMON_FLAGS)
LNK_FLAGS = -shared

# Final binary
BIN = HexDump.so

# Put all auto generated stuff to this build dir.
BUILD_DIR = ./build

SOURCE_DIR = ..

SRC_DIR = src
LIBS_DIR = ..

# List of all .c source files.
SOURCE = $(SRC_DIR)/HexDump.cpp \
	$(SRC_DIR)/BPDS.c \
	$(SRC_DIR)/ColorStream.cpp \

INCLUDES = ../src \
	../../../../WhippyTerm/src

# All .o files go to build dir.
C_OBJ = $(SOURCE:%.c=$(BUILD_DIR)/%.o)
CXX_OBJ = $(C_OBJ:%.cpp=$(BUILD_DIR)/%.o)
OBJ = $(CXX_OBJ)

# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)
# Include paths with a -I in front of them
CC_INCLUDE = $(INCLUDES:%= -I %)

# Default target named after the binary.
$(BIN) : $(BUILD_DIR)/$(BIN)

# Actual target of the binary - depends on all .o files.
$(BUILD_DIR)/$(BIN): $(OBJ)
	echo Linking...
	# Create build directories - same structure as sources.
	mkdir -p $(@D)
	# Just link all the object files.
	$(CC) $(CCXX_FLAGS) $(OBJ) $(LNK_FLAGS) -o $@
	-cp $(BUILD_DIR)/$(BIN) $(BIN)

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.cpp
	echo Compiling $(notdir $<)
	mkdir -p $(@D)
	# The -MMD flags additionaly creates a .d file with
	# the same name as the .o file.
	$(CCXX) $(CCXX_FLAGS) $(CC_INCLUDE) -MMD -c $< -o $@

$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.c
	echo Compiling $(notdir $<)
	mkdir -p $(@D)
	# The -MMD flags additionaly creates a .d file with
	# the same name as the .o file.
	$(CC) $(CC_FLAGS) $(CC_INCLUDE) -MMD -c $< -o $@

#.PHONY : clean
clean:
	# This should remove all generated files.
	-rm -rf $(BUILD_DIR)/

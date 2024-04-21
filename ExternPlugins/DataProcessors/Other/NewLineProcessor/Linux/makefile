CC = g++
# add -g for debugging info
CC_FLAGS = -Wall -fmax-errors=1 -Wfatal-errors -Wno-memset-transposed-args -pthread -fPIC
LNK_FLAGS = -shared

# Final binary
BIN = NewLineProcessor.so

# Put all auto generated stuff to this build dir.
BUILD_DIR = ./build

SOURCE_DIR = ..

SRC_DIR = src
LIBS_DIR = ..

# List of all .c source files.
SOURCE = $(SRC_DIR)/NewLineProcessor.cpp \

INCLUDES = src \
	../../../../../src

# All .o files go to build dir.
OBJ = $(SOURCE:%.c=$(BUILD_DIR)/%.o)
OBJ = $(SOURCE:%.cpp=$(BUILD_DIR)/%.o)
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
	$(CC) $(CC_FLAGS) $(OBJ) $(LNK_FLAGS) -o $@
	-cp $(BUILD_DIR)/$(BIN) $(BIN)
#	-$(CC) $(CC_FLAGS) $^ -o $@ 2>tmp.err
#	head tmp.err

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
	$(CC) $(CC_FLAGS) $(CC_INCLUDE) -MMD -c $< -o $@

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

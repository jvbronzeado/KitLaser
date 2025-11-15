CXX := g++
CXXFLAGS := -O3 -std=c++17 -Iinclude

OUTPUT_DIR := bin
OBJ_DIR := obj
EXECUTABLE_NAME := kit
EXECUTABLE_PATH := $(OUTPUT_DIR)/$(EXECUTABLE_NAME)

SRCS := $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

DEPS := $(OBJS:.o=.d)

# default target
all: $(OUTPUT_DIR) $(OBJ_DIR) $(EXECUTABLE_PATH)

# include generated dependency files
-include $(DEPS)

# guarantees output directory exists
$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# guarantees objects directory exists
$(OBJ_DIR):
	echo $(OBJS)
	mkdir -p $(OBJ_DIR)

# link all object files into final executable
$(EXECUTABLE_PATH): $(OBJS)
	$(CXX) $(OBJS) -o $@

# compile .cpp files into obj/*.o
$(OBJ_DIR)/%.o: src/%.cpp
	echo $@
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

# clear target erasing output file
clean:
	rm -rf $(OUTPUT_DIR)
	rm -rf $(OBJ_DIR)

run:
	./$(EXECUTABLE_PATH)

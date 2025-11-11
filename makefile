CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude

OUTPUT_DIR := bin
EXECUTABLE_NAME := kit
EXECUTABLE_PATH := $(OUTPUT_DIR)/$(EXECUTABLE_NAME)

SRCS := $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, $(OUTPUT_DIR)/%.o, $(SRCS))

DEPS := $(OBJS:.o=.d)

# default target
all: $(OUTPUT_DIR) $(EXECUTABLE_PATH)

# include generated dependency files
-include $(DEPS)

# guarantees output directory exists
$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# link all object files into final executable
$(EXECUTABLE_PATH): $(OBJS)
	$(CXX) $(OBJS) -o $@

# compile .cpp files into bin/*.o
$(OUTPUT_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

# clear target erasing output file
clean:
	rm -rf $(OUTPUT_DIR)

run:
	./$(EXECUTABLE_PATH)

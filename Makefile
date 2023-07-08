# Compiler
CXX = clang++

# Source files
SRC_DIR = ./src
SHAPE_SRC = Rectangle.cpp
SHAPE_SRC := $(addprefix /shapes/, $(SHAPE_SRC))
SRC := $(addprefix $(SRC_DIR)/, $(SHAPE_SRC))
SRC += $(SRC_DIR)/main.cpp

# Release and debug object files
OBJ_DIR = .build
RELEASE_OBJ := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
DBG_OBJ := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=_dbg.o))

# Warning and include flags
WRN = -Wall -Wextra -Werror -Wpedantic -Wcast-align -Wunused -Wshadow \
			-Wcast-qual -Wmissing-prototypes -Wno-missing-braces -std=c++98
INC = -Iinclude -Iinclude/math -Iinclude/shapes -Iinclude/Network -Itests
CXXFLAGS = $(WRN) $(INC)

# Release and debug flags
DBG_BUILD = webserv
DEBUG_FLAGS = -g3 -fno-omit-frame-pointer -DDEBUG
RELEASE_BUILD = prod_webserv
RELEASE_FLAGS = -Ofast -march=native -funroll-loops -finline-functions \
				-fvectorize -DNDEBUG

# If valgrind is not available use the address sanitizer instead
ifeq (, $(shell which valgrind))
	DEBUG_FLAGS += -fsanitize=address,undefined
endif

# Compile database for use with clangd
COMPILE_DB = compile_commands.json

# Dependency files needed for recompiling with header file changes
DEPENDS := $(RELEASE_OBJ:.o=.d) $(DBG_OBJ:.o=.d)

# Unit tests
TEST_SRC := $(filter-out $(SRC_DIR)/main.cpp, $(SRC)) tests/shape_tests.cpp
TEST_OBJ := $(addprefix $(OBJ_DIR)/, $(TEST_SRC:.cpp=.o))
TEST = run_tests

# Build debug and release executables
all: $(RELEASE_BUILD) $(DBG_BUILD) db

# Run release build
run: $(RELEASE_BUILD) db
	./$(RELEASE_BUILD)

# Run debug build
dbg: $(DBG_BUILD) db
	./$(DBG_BUILD)

# Release objs
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS)  -MMD -MP -c -o $@ $<

# Debug objs
$(OBJ_DIR)/%_dbg.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) -MJ $@.json $(CXXFLAGS) $(DEBUG_FLAGS) -MMD -MP -c -o $@ $<

# Link release build
$(RELEASE_BUILD): $(RELEASE_OBJ) Makefile
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(RELEASE_OBJ) $(LINK_FLAGS) -o $(RELEASE_BUILD)

# Link debug build
$(DBG_BUILD): $(DBG_OBJ) Makefile
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) $(DBG_OBJ) $(LINK_FLAGS) -o $(DBG_BUILD)

# Include dependencies needed to recompile on header file changes
-include $(DEPENDS)

# Create test executable
$(TEST): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(TEST_OBJ) $(LINK_FLAGS) -o $(TEST)

# Run unit tests
test: $(TEST)
	./$(TEST)

# Create compile db
db: $(COMPILE_DB)

$(COMPILE_DB): Makefile
	sed -e '1s/^/[\'$$'\n''/' -e '$$s/,$$/\'$$'\n'']/' $(OBJ_DIR)/src/**/*.o.json $(OBJ_DIR)/src/*.o.json > compile_commands.json

# Remove object files
clean:
	rm -rf $(OBJ_DIR)

# Remove object files and builds
fclean: clean
	rm -f $(RELEASE_BUILD) $(DBG_BUILD) $(TEST)

# Creates and hosts doxygen documentation
docs:
	doxygen
	make html -C docs/sphinx
	sleep 2 && python3 -m webbrowser -t "http://localhost:8000"
	python3 -m http.server --directory docs/sphinx/build/html

# Remove object files and builds and re-compile release and debug builds
re: fclean $(RELEASE_BUILD) $(DBG_BUILD) db

.PHONY: all re fclean clean run dbg db docs test

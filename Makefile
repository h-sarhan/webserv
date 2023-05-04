CXX = clang++

SHAPE_SRC = Rectangle.cpp
SHAPE_SRC := $(addprefix /shapes/, $(SHAPE_SRC))

SRC_DIR = ./src
SRC = main.cpp $(SHAPE_SRC)
SRC := $(addprefix $(SRC_DIR)/, $(SRC))

OBJ_DIR = .obj

RELEASE_OBJ := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
DBG_OBJ := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=_dbg.o))

RELEASE_BUILD = prod_webserv
DBG_BUILD = webserv

WRN = -Wall -Wextra -Werror -Wpedantic -Wcast-align -Wunused -Wshadow \
			-Wcast-qual -Wmissing-prototypes -Wno-missing-braces
INC = -Iinclude -Iinclude/math -Iinclude/shapes -Itests
CXXFLAGS = $(WRN) $(INC)

RELEASE_FLAGS = -Ofast -march=native -flto -funroll-loops \
					-finline-functions -fvectorize -DNDEBUG

DEBUG_FLAGS = -g3 -fno-omit-frame-pointer -DDEBUG
ifeq (, $(shell which valgrind))
	DEBUG_FLAGS += -fsanitize=address,undefined,
endif

COMPILE_DB = compile_commands.json

DEPENDS := $(RELEASE_OBJ:.o=.d) $(DBG_OBJ:.o=.d)

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
	$(CXX) -MJ $@.json $(CXXFLAGS) $(RELEASE_FLAGS)  -MMD -MP -c -o $@ $<

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

-include $(DEPENDS)

# Create compile db
db: $(COMPILE_DB)

$(COMPILE_DB): Makefile
	sed -e '1s/^/[\'$$'\n''/' -e '$$s/,$$/\'$$'\n'']/' .obj/src/*.o.json > compile_commands.json

# Remove object files
clean:
	rm -rf $(OBJ_DIR)

# Remove object files and builds
fclean: clean
	rm -f $(RELEASE_BUILD) $(DBG_BUILD)

# Remove object files and builds and re-compile release and debug builds
re: fclean $(RELEASE_BUILD) $(DBG_BUILD)

.PHONY: all re fclean clean run dbg db

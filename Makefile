# Windows Makefile needs to be checked
ifeq ($(OS),Windows_NT)
    OS_NAME = Windows
    TARGET = $(BUILD_DIR)/main.exe
    
    ifeq ($(MINGW_PREFIX),)
        PREFIX = $(subst /bin/g++.exe,,$(shell where g++))
    else
        PREFIX = $(MINGW_PREFIX)
    endif

    SAFE_PREFIX = $(subst \,/,$(PREFIX))

    CXXFLAGS = -g -std=c++17 -Wa,-mbig-obj
    INCLUDES = -I "$(SAFE_PREFIX)/include" -I src
    LIBS = -L "$(SAFE_PREFIX)/lib" -lsfml-graphics -lsfml-window -lsfml-system
    
    CLEAN_CMD = rm -rf $(BUILD_DIR)/*.o $(TARGET)
    MKDIR_CMD = if not exist $(subst /,\,$(BUILD_DIR)) mkdir $(subst /,\,$(BUILD_DIR))
# Added Linux Makefile
else
    OS_NAME = Linux
    TARGET = $(BUILD_DIR)/main
    CXXFLAGS = -g -std=c++17
    INCLUDES = -I src
    LIBS = -lsfml-graphics -lsfml-window -lsfml-system
    CLEAN_CMD = rm -rf $(BUILD_DIR)/*.o $(TARGET)
    MKDIR_CMD = mkdir -p $(BUILD_DIR)
endif

CXX = g++
SRC_DIR = src
BUILD_DIR = build
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: print_info $(TARGET)

print_info:
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR_CMD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(CLEAN_CMD)

.PHONY: all clean print_info

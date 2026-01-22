CXX = g++
CXXFLAGS = -g -std=c++17 -Wa,-mbig-obj

INCLUDES = -I E:/msys2/install/ucrt64/include -I src
LIBS = -L E:/msys2/install/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR = src
BUILD_DIR = build

TARGET = $(BUILD_DIR)/main.exe

SRCS = $(wildcard $(SRC_DIR)/*.cpp)

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)

.PHONY: all clean
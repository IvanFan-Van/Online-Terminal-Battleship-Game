# 编译器
CXX = g++
# 标志
FLAGS = -pedantic-errors -std=c++17 -static -MMD -MP -Wall -g

# 基本构建配置
SRC_DIR = ./src
BUILD_DIR = ./build
INCLUDE_DIR = ./include

# 找到所有依赖文件
DEP_FILES = $(wildcard $(BUILD_DIR)/*.d)

# 目标
TARGETS = main server

.PHONY: all clean test

all: $(TARGETS)

# 海战棋游戏
GAME_OBJECTS = $(patsubst $(SRC_DIR)/battleship/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/battleship/*.cpp))
# 海战棋客户端
CLIENT_OBJECTS = $(patsubst $(SRC_DIR)/client/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/client/*.cpp))
# 海战棋服务器
SERVER_OBJECTS = $(patsubst $(SRC_DIR)/server/%.cpp, $(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/server/*.cpp))

COMMON_OBJECTS = $(patsubst $(SRC_DIR)/common/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/common/*.cpp))

# 包含所有依赖文件
-include $(DEP_FILES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/**/%.cpp
	$(CXX) $(FLAGS) -I $(INCLUDE_DIR) -c $< -o $@


# 构建main
main: $(BUILD_DIR)/main

$(BUILD_DIR)/main: ./src/main.cpp $(GAME_OBJECTS) $(CLIENT_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(FLAGS) -I ./include $^ -pthread -o $@

server: $(BUILD_DIR)/server

$(BUILD_DIR)/server: $(SERVER_OBJECTS) $(GAME_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(FLAGS) -I ./include $^ -pthread -o $@

# clean
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d

TEST_TARGETS = $(patsubst ./test/%.cpp, ./build/%, $(wildcard ./test/*.cpp))

test: $(TEST_TARGETS)

./build/test_%: ./test/test_%.cpp $(GAME_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(FLAGS) -I ./include $^ -o $@
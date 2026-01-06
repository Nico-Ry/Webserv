# Webserv Makefile - C++98 compliant

NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I./include/network

# Directories
SRC_DIR = src/network
OBJ_DIR = obj

# Source files
SRCS = $(SRC_DIR)/SocketManager.cpp \
       $(SRC_DIR)/Connection.cpp \
       $(SRC_DIR)/IOMultiplexer.cpp \
       $(SRC_DIR)/Server.cpp

# Main source
MAIN_SRC = src/main.cpp

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
MAIN_OBJ = $(OBJ_DIR)/main.o

# Colors
GREEN = \033[0;32m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS) $(MAIN_OBJ)
	@echo "$(GREEN)Building $(NAME)...$(RESET)"
	$(CXX) $(CXXFLAGS) $(OBJS) $(MAIN_OBJ) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled successfully!$(RESET)"
	@echo "$(GREEN)Run with: ./$(NAME) [port]$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning object files..."
	rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Cleaning binaries..."
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
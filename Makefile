# Webserv Makefile - C++98 compliant

NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I./include

# Directories
OBJ_DIR = obj

# Source files
SRC_NETWORK = src/network/SocketManager.cpp \
              src/network/Connection.cpp \
              src/network/IOMultiplexer.cpp \
              src/network/Server.cpp

SRC_HTTP = src/http/RequestParser.Core1.cpp \
           src/http/RequestParser.StartLine3.cpp \
           src/http/RequestParser.Headers4.cpp \
           src/http/RequestParser.Body5.cpp \
           src/http/RequestParser.Utils2.cpp \
           src/http/RequestParser.Connection6.cpp \
           src/http/ResponseBuilder7.cpp \
           src/http/Mime8.cpp

SRCS = $(SRC_NETWORK) $(SRC_HTTP)

# Main source
MAIN_SRC = src/main.cpp

# Object files
OBJS = $(SRCS:src/%.cpp=$(OBJ_DIR)/%.o)
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

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
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
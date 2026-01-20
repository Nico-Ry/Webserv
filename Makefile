# Webserv Makefile - C++98 compliant

# Colors
RESET			= "\033[0m"	# Default
BLACK    		= "\033[30m"	# Black
RED      		= "\033[31m"	# Red
GREEN    		= "\033[32m"	# Green
YELLOW   		= "\033[33m"	# Yellow
BLUE     		= "\033[34m"	# Blue
MAGENTA  		= "\033[35m"	# Magenta
CYAN     		= "\033[36m"	# Cyan
WHITE    		= "\033[37m"	# White


NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I./include

# Directories
OBJ_DIR = obj

# Source files
SRC_NETWORK 		= src/network/SocketManager.cpp \
            		  src/network/Connection.cpp \
            		  src/network/IOMultiplexer.cpp \
            		  src/network/Server.cpp

SRC_HTTP 			= src/http/RequestParser.Core1.cpp \
         			  src/http/RequestParser.StartLine3.cpp \
         			  src/http/RequestParser.Headers4.cpp \
        			  src/http/RequestParser.Body5.cpp \
        			  src/http/RequestParser.Utils2.cpp \
        			  src/http/RequestParser.Connection6.cpp \
        			  src/http/ResponseBuilder7.cpp \
        			  src/http/Mime8.cpp


CP_DIR				= src/configParser/
SRC_CONFIG_PARSER	= $(addprefix $(CP_DIR), \
						Config.cpp \
						ConfigParser.cpp \
						LocationBlock.cpp \
						parseLocationBlock.cpp \
						parseServerBlock.cpp \
						ServerBlock.cpp \
						Tokeniser.cpp)

ROUTER_DIR			= src/router/
SRC_ROUTER			= $(addprefix $(ROUTER_DIR), \
						Router.cpp \
						)


SRC_DEBUG			= src/debug/configParserUtils.cpp \
					  src/debug/httpParserUtils.cpp


SRCS = $(SRC_NETWORK) $(SRC_HTTP) $(SRC_CONFIG_PARSER) $(SRC_ROUTER) $(SRC_DEBUG)

# Main source
MAIN_SRC = src/main.cpp

# Object files
OBJS = $(SRCS:src/%.cpp=$(OBJ_DIR)/%.o)
MAIN_OBJ = $(OBJ_DIR)/main.o

# Colors
#GREEN = \033[0;32m


all: $(NAME)

$(NAME): $(OBJS) $(MAIN_OBJ)
	@echo $(CYAN)   " - ⏳ Making $(NAME)..." $(RESET)
#	@echo "$(GREEN)Building $(NAME)...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) $(MAIN_OBJ) -o $(NAME)
	@echo $(GREEN)  " - ✅ $(NAME) Ready! " $(RESET)
#	@echo "$(GREEN)Run with: ./$(NAME) [port]$(RESET)"
	@echo "To get started type: 	" $(CYAN) "./$(NAME) "$(RESET)"<config_file>"

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@echo $(YELLOW) " - 🛠️  Compiling $<..." $(RESET)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	@echo $(YELLOW) " - 🛠️  Compiling $<..." $(RESET)
#	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
#	@echo "Cleaning object files..."
	@echo $(MAGENTA)" - 🗑️  Object Files Deleted  ✅"   $(RESET)
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo $(GREEN)  " - 🗑️  Fully Cleaned!  ✅" $(RESET)
#	@echo "Cleaning binaries..."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
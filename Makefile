# Colors
RESET			= "\033[0m"
BLACK    		= "\033[30m"    # Black
RED      		= "\033[31m"    # Red
GREEN    		= "\033[32m"    # Green
YELLOW   		= "\033[33m"    # Yellow
BLUE     		= "\033[34m"    # Blue
MAGENTA  		= "\033[35m"    # Magenta
CYAN     		= "\033[36m"    # Cyan
WHITE    		= "\033[37m"    # White

# Compiler and flags
CXX			= c++ -Iconfig_parser/include
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

# Create directories if they don't exist
MKDIR		= mkdir -p

# Target executable name
NAME		= Webserv

# Header files
INCL_DIR	= config_parser/include/
INCLUDES	= $(addprefix $(INCL_DIR), Config.hpp ConfigParser.hpp ServerBlock.hpp LocationBlock.hpp Tokeniser.hpp utils.hpp colours.hpp)


# Source files
SRCS_DIR	= config_parser/srcs/
SRC			= $(addprefix $(SRCS_DIR), Config.cpp ConfigParser.cpp ServerBlock.cpp LocationBlock.cpp Tokeniser.cpp main.cpp utils.cpp)

# Object files (not strictly needed if just one .cpp file, but useful for scaling)
OBJ_FILES	= $(SRC:.cpp=.o)
OBJS = $(addprefix $(OBJS_DIR), $(notdir $(OBJ_FILES)))
OBJS_DIR	= config_parser/objs/


# Pattern rule to compile .cpp to .o
$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp $(INCLUDES)
	@echo $(YELLOW) " - 🛠️  Compiling $<..." $(RESET)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<


# Default target
all: $(OBJS_DIR) $(NAME)

$(OBJS_DIR):
	@$(MKDIR) $(OBJS_DIR)

$(NAME): $(OBJS)
	@echo $(CYAN)   " - ⏳ Making $(NAME)..." $(RESET)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo $(GREEN)  " - ✅ $(NAME) Ready! " $(RESET)
	@echo "To get started type: 	" $(CYAN) "./$(NAME)" $(RESET)


# Clean object files
clean:
	@rm -rf $(OBJS_DIR)
	@echo $(MAGENTA)" - 🗑️  Object Files Deleted  ✅"   $(RESET)


# Clean everything including the executable
fclean: clean
	@rm -f $(NAME)
	@echo $(GREEN)  " - 🗑️  Fully Cleaned!  ✅" $(RESET)


# Recompile from scratch
re: fclean all

.PHONY: all clean fclean re

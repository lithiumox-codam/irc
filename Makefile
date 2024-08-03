NAME			:= ircserv
NICKNAME		:= IRC

# Directories
HDR_DIR			:= include
LIB_DIR			:= lib
SRC_DIR			:= src
OBJ_DIR			:= build

# Compiler flags
CC				:= c++
CFLAGS			:= -Wall -Werror -Wextra -std=c++20 -I./$(HDR_DIR) -I $(LIB_DIR)

ifdef DEBUG
	CFLAGS		+= -g -fsanitize=address
endif

# Includes
HDR		:= $(wildcard $(HDR_DIR)/*.hpp)

# Files
SRC		:=	$(wildcard $(SRC_DIR)/*.cpp)

OBJ				:= $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.cpp=.o)))

# Colours
GREEN			:= \033[32;1m
YELLOW			:= \033[33;1m
RED				:= \033[31;1m
BOLD			:= \033[1m
RESET			:= \033[0m

# Rules
all: ${NAME}

$(NAME): $(OBJ)
	@ printf "%b%s%b" "$(YELLOW)$(BOLD)" "Compiling $(NICKNAME)..." "$(RESET)"
	@ $(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@ printf "\t\t%b%s%b\n" "$(GREEN)$(BOLD)" "[OK]" "$(RESET)"


$(OBJ_DIR)/%.o: src/%.cpp $(HDR)
	@ mkdir -p $(OBJ_DIR)
	@ $(CC) $(CFLAGS) -c $< -o $@ -I $(HDR_DIR)

open: $(NAME)
	@ ./$(NAME) 8080

clean:
	@ echo "$(RED)$(BOLD)Cleaning $(NICKNAME)...$(RESET)"
	@ rm -rf $(OBJ_DIR)

fclean:
	@ echo "$(RED)$(BOLD)Fully cleaning $(NICKNAME)...$(RESET)"
	@ rm -rf $(OBJ_DIR)
	@ rm -rf ${NAME}

re: fclean ${NAME}

format:
	@ clang-format -i $(SRC) $(HDR)

run: all
	@ ./$(NAME) 8080 test

bear: fclean
	@ bear -- make

.PHONY: all clean fclean re open

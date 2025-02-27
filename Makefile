NAME			:= ircserv
NICKNAME		:= IRC

# Directories
HDR_DIR			:= include
LIB_DIR			:= lib
SRC_DIR			:= src
OBJ_DIR			:= build

BONUS_DIR		:= bot

# Compiler flags
CC				:= c++
CFLAGS			:= -Wall -Werror -Wextra -std=c++20 -I./$(HDR_DIR) -I $(LIB_DIR)

ifdef DEBUG
	CFLAGS		+= -g -fsanitize=address
endif

# Includes
HDR		:= $(shell find $(HDR_DIR) -name '*.hpp')

# Files
SRC		:= $(shell find $(SRC_DIR) -name '*.cpp')
OBJ		:= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDR)
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) -c $< -o $@ -I $(HDR_DIR)

open: $(NAME)
	@ ./$(NAME) 8080

bonus:
	@ make run -C $(BONUS_DIR)

clean:
	@ echo "$(RED)$(BOLD)Cleaning $(NICKNAME)...$(RESET)"
	@ rm -rf $(OBJ_DIR)
	@ make clean -C $(BONUS_DIR)

fclean:
	@ echo "$(RED)$(BOLD)Fully cleaning $(NICKNAME)...$(RESET)"
	@ rm -rf $(OBJ_DIR)
	@ rm -rf ${NAME}
	@ make fclean -C $(BONUS_DIR)

re: fclean ${NAME}

format:
	@ clang-format -i $(SRC) $(HDR)

run: all
	@ ./$(NAME) 6667 test

bear: fclean
	@ bear -- make

.PHONY: all clean fclean re open format run bear bonus

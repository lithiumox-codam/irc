NAME			:= ircbot
NICKNAME		:= BOT

# Directories
HDR_DIR			:= include
SRC_DIR			:= src
OBJ_DIR			:= build

# Compiler flags
CC				:= c++
CFLAGS			:= -Wall -Werror -Wextra -std=c++20

# Add other flags as necessary
ifdef DEBUG
    CFLAGS += -g -fsanitize=address
endif

# Linker flags
CURL_H	:= /home/opelser/.capt/root/usr/include/x86_64-linux-gnu
LDFLAGS	:= /lib/x86_64-linux-gnu/libcurl.so.4

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
all: update_submodules ${NAME}

update_submodules:
	@ echo "$(YELLOW)Checking and updating git submodules...$(RESET)"
	@ git submodule update --init --recursive
	@ echo "$(GREEN)Submodules are up to date!$(RESET)"

$(NAME): $(OBJ)
	@ printf "%b%s%b" "$(YELLOW)$(BOLD)" "Compiling $(NICKNAME)..." "$(RESET)"
	@ $(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) -o $(NAME)
	@ printf "\t\t%b%s%b\n" "$(GREEN)$(BOLD)" "[OK]" "$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDR)
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) -c $< -o $@ -I $(HDR_DIR) -I $(CURL_H)

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
	@ ./$(NAME) localhost 6667 test

bear: fclean
	@ bear -- make

.PHONY: all clean fclean re open format run bear

NAME = ircserver
SRC = src/main.cpp src/options.cpp
HEADERS = $(wildcard "includes/*.hpp")
BUILD = build
OBJ = $(SRC:src/%.cpp=$(BUILD)/%.o)
SUBDIRS = $(wildcard src/*)

all: $(NAME)

$(NAME): $(OBJ) $(BUILD_DIR) $(HEADERS) | $(BUILD)
	@echo "Linking $@"
	@g++ $(OBJ) -o $(NAME)

$(BUILD)/%.o: src/%.cpp $(HEADERS) | $(BUILD)
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@g++ -c $< -o $@ -I includes

$(BUILD):
	@mkdir -p $(BUILD)

clean:
	@rm -rf $(BUILD_DIR)

fclean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(NAME)

re: fclean all

bear: fclean
	@bear -- make all

format:
	@clang-format -i $(SRC) $(HEADERS)

.PHONY: all clean fclean re $(SUBDIRS) format

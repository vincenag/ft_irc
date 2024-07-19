NAME = ircserv

CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g
RM = rm -rf

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc

SRCS := main.cpp\
		$(SRC_DIR)/Server.cpp\
		$(SRC_DIR)/Client.cpp\
		$(SRC_DIR)/ArgumentsValidator.cpp\
		$(SRC_DIR)/Channel.cpp\
		$(SRC_DIR)/CommandHandler.cpp\
		$(SRC_DIR)/Utiles.cpp\
		$(SRC_DIR)/IRCBot.cpp\

OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

#========================= COLORS =========================#

GREEN   = \033[92;5;118m
YELLOW  = \033[93;5;226m
CIAN    = \033[36m
BLUE    = \033[1;38;5;27m
END     = \033[0m


#========================= ARGS =========================#

all: $(NAME)

show:
	@printf "NAME   : $(NAME)\n"
	@printf "CC     : $(CC)\n"
	@printf "FLAGS  : $(FLAGS)\n"
	@printf "SRCS   : $(addprefix \n\t , $(SRCS))\n"
	@printf "OBJS   : $(addprefix \n\t , $(OBJS))\n"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(YELLOW)- Object files removed.$(END)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(YELLOW)- Executable removed.$(END)"

re: fclean all

$(NAME): $(OBJS) | $(OBJ_DIR)
	@echo "Compiling with flags: $(FLAGS)"
	$(CC) $(FLAGS) -I inc $(OBJS) -o $(NAME)
	@echo "$(GREEN)\n[Compiled$(CIAN) $@ $(GREEN)successfully]$(END)"

#========================= RULES =========================#

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(FLAGS) -I inc -c $< -o $@

$(OBJ_DIR)/main.o: main.cpp
	mkdir -p $(dir $@)
	$(CC) $(FLAGS) -I inc -c $< -o $@

.PHONY: all show clean fclean re
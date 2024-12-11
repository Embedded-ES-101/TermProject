CC			= gcc
RM			= rm -rf
NAME		= ./user_interface
NAME_SHORT	= user_interface

INCS_DIR	= .
MAIN_INC	= $(foreach D, $(INCS_DIR), -I$(D))

SRCS_DIR 	= .
SRCS		= 	client.c\
				banner.c\

OBJS		= $(SRCS:.c=.o)

_COLOR		= \033[32m
_BOLDCOLOR	= \033[32;1m
_RESET		= \033[0m
_CLEAR		= \033[0K\r\c
_OK			= [\033[32mOK\033[0m]

%.o			: %.cpp
			@echo "[..] $(NAME_SHORT)... compiling $*.c\r\c"
			@$(CC) $(MAIN_INC) -c $< -o $@
			@echo "$(_CLEAR)"

all			: $(NAME)

$(NAME)		: $(OBJS) $(INCS)
			@$(CC) $(OBJS) $(MAIN_INC) -o $(NAME)
			@echo "$(_OK) $(NAME_SHORT) compiled"

clean		:
			@$(RM) 

fclean		: clean
			@$(RM) $(NAME)

re			: fclean all

.PHONY		: all clean fclean re
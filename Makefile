NAME = ircserv

S			= srcs/
I			= includes/

SRC = $S/main.cpp $S/Server.cpp

FLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address
INCLUDES	= -I$I

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(SRC)
	@c++ $(FLAGS) $(INCLUDES) -o $(NAME) $(SRC)

clean:
	@rm -f $(NAME)

fclean: clean
	@rm -f $(NAME) client

re: fclean all

debug: FLAGS += -g
debug: re

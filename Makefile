#  Copyright 2024 <faboussa>************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/24 21:33:43 by mbernard          #+#    #+#              #
#    Updated: 2024/11/25 16:21:47 by faboussa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

C = c++
NAME = ircserv
NAME_BOT = ircbot
CFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98
MKDIR = mkdir -p
RMDIR = rm -rf

# ---------------------------------- Sources --------------------------------- #
vpath %.cpp src src/commands src/bonus

HEADERS_LIST =  Server Config Client Channel Parser Bot \
                numericReplies utils \
                colors enums

SRCS = main Server Client Channel Parser Config \
       numericReplies messageManagement utils \
       pass nick user \
	   invite join kick list mode part ping privmsg quit topic who \
	   botCommands
# ---------------------------------- Répertoires ----------------------------- #
HEADERS_DIR = includes/
OBJS_DIR = .objs/
OBJS = ${addprefix ${OBJS_DIR}, ${addsuffix .o, ${SRCS}}}
#HEADERS = ${addprefix ${HEADERS_DIR}, ${addsuffix .hpp, ${HEADER_LIST}}}
INCLUDES = -I ${HEADERS_DIR}
DEPS = ${OBJS:.o=.d}
HEADERS = ${addprefix ${HEADERS_DIR}, ${addsuffix .hpp, ${HEADERS_LIST}}}

# ---------------------------------- Bot ------------------------------------- #
HEADERS_LIST_BOT = Bot Log utils colors enums
SRCS_BOT = mainBot Bot Log botLog handleClientRequests handleApiResponse \
           utils

OBJS_DIR_BOT = .objs_bot/
OBJS_BOT = ${addprefix ${OBJS_DIR_BOT}, ${addsuffix .o, ${SRCS_BOT}}}
INCLUDES_BOT = $(addprefix -I$(HEADERS_DIR), $(HEADERS_LIST_BOT))
HEADERS_BOT = ${addprefix ${HEADERS_DIR}, ${addsuffix .hpp, ${HEADERS_LIST_BOT}}}

# ---------------------------------- Compilation ----------------------------- #
all: create_dirs ${NAME}

${NAME}: ${OBJS} Makefile
	${C} ${CFLAGS} ${OBJS} ${INCLUDES} -o $@

${OBJS_DIR}%.o: %.cpp ${HEADERS} Makefile | ${OBJS_DIR}
	${C} ${CFLAGS} ${INCLUDES} -c $< -o $@

create_dirs:
	@${foreach dir, ${sort ${dir ${OBJS}}}, ${MKDIR} ${dir};}

-include ${DEPS}

# ---------------------------------- Compilation  Bot ------------------------ #
create_dirs_bot:
	@${MKDIR} ${OBJS_DIR_BOT}

bot: create_dirs_bot ${NAME_BOT}

${NAME_BOT}: ${OBJS_BOT} Makefile
	${C} ${CFLAGS} ${OBJS_BOT} ${INCLUDES_BOT} -o $@

${OBJS_DIR_BOT}%.o: %.cpp ${HEADERS} Makefile | ${OBJS_DIR_BOT}
	${C} ${CFLAGS} ${INCLUDES_BOT} -c $< -o $@

# ---------------------------------- Create Repertory ------------------------ #
${OBJS_DIR}:
			${MKDIR} ${OBJS_DIR}

# ---------------------------------- Debug ----------------------------------- #
debug: CFLAGS := ${filter-out -Werror, ${CFLAGS}}
debug: C = g++
debug: CFLAGS += -DDEBUG -g3
debug: clean create_dirs ${NAME}

#----------------------------------- Fsanitize ------------------------------- #
fsan:
	$(RMDIR) $(DIR_OBJS)
	$(MAKE) CFLAGS="-g3 -fsanitize=address"
	$(MAKE) clean
# ---------------------------------- Valgrind -------------------------------- #
valgrind: $(NAME) debug
			valgrind --track-fds=yes --leak-check=full \
			--show-leak-kinds=all -s ./$(NAME) 6667 pass

# ---------------------------------- Tests ----------------------------------- #
testnumericr: CFLAGS += -DTESTNUMERICR
testnumericr: fclean ${OBJS_DIR} ${NAME}

testlist: CFLAGS += -g3 -DTESTLIST
testlist: fclean ${OBJS_DIR} ${NAME}

# ---------------------------------- Clean ----------------------------------- #
clean:
	${RMDIR} ${OBJS_DIR}

fclean: clean
	${RM} ${NAME}

re: fclean all

# ---------------------------------- Phony ----------------------------------- #
.PHONY: all clean fclean re debug fsanitize valgrind fsan

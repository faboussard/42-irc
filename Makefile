#  Copyright 2024 <faboussa>************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/24 21:33:43 by mbernard          #+#    #+#              #
#    Updated: 2024/11/25 16:03:49 by faboussa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

C = c++
NAME = ircserv
CFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98
MKDIR = mkdir -p
RMDIR = rm -rf

# ---------------------------------- Sources --------------------------------- #
vpath %.cpp src src/commands src/bonus

HEADERS_LIST =  Server Config Client Channel Parser Bot \
                numericReplies utils \
                colors types

SRCS = main Server Client Channel Parser Config Bot\
       numericReplies messageManagement utils \
       pass nick user \
	   invite join kick list mode part ping privmsg quit topic who \
	   botCommands

# ---------------------------------- Répertoires ----------------------------- #
HEADERS_DIR = includes/
OBJS_DIR = .objs/
OBJS = ${addprefix ${OBJS_DIR}, ${addsuffix .o, ${SRCS}}}
HEADERS = ${addprefix ${HEADERS_DIR}, ${addsuffix .hpp, ${HEADER_LIST}}}
INCLUDES = -I ${HEADERS_DIR}
DEPS = ${OBJS:.o=.d}
HEADERS = ${addprefix ${HEADERS_DIR}, ${addsuffix .hpp, ${HEADERS_LIST}}}

# ---------------------------------- Compilation ----------------------------- #
all: create_dirs ${NAME}

${NAME}: ${OBJS} Makefile
	${C} ${CFLAGS} ${OBJS} ${INCLUDES} -o $@

${OBJS_DIR}%.o: %.cpp ${HEADERS} Makefile | ${OBJS_DIR}
	${C} ${CFLAGS} ${INCLUDES} -c $< -o $@

create_dirs:
	@${foreach dir, ${sort ${dir ${OBJS}}}, ${MKDIR} ${dir};}

-include ${DEPS}

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
# ---------------------------------- valgrind -------------------------------- #
valgrind: $(NAME) debug
			valgrind --track-fds=yes --leak-check=full \
			--show-leak-kinds=all -s ./$(NAME) 6667 pass

# ---------------------------------- fsanitize ------------------------------- #
fsanitize: C = g++
fsanitize: CFLAGS += -fsanitize=address -g
fsanitize: clean create_dirs ${NAME}

# ---------------------------------- Test ------------------------------------ #
test: CFLAGS := $(filter-out -Werror, $(CFLAGS))
test: CFLAGS += -DTEST
test: clean create_dirs ${NAME}

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

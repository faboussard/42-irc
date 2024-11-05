#  Copyright 2024 <faboussa>************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/24 21:33:43 by mbernard          #+#    #+#              #
#    Updated: 2024/11/05 11:25:28 by faboussa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

C = c++
NAME = ircserv
CFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98
MKDIR = mkdir -p
RMDIR = rm -rf

# ---------------------------------- Sources --------------------------------- #
vpath %.cpp src src/commands

HEADERS_LIST = colors Server Client Channel Parser numericReplies utils serverConfig tests
SRCS = main Server Client Channel Parser numericReplies utils pass nick user invite mode ping topic join unitTests/join unitTests/general

# ---------------------------------- Répertoires ----------------------------- #
HEADERS_DIR = includes/
OBJS_DIR = .objs/
OBJS = $(addprefix ${OBJS_DIR}, $(addsuffix .o, ${SRCS}))
HEADERS = $(addprefix ${HEADERS_DIR}, $(addsuffix .hpp, ${HEADER_LIST}))
INCLUDES = -I ${HEADERS_DIR}
DEPS = ${OBJS:.o=.d}
HEADERS = $(addprefix ${HEADERS_DIR}, $(addsuffix .hpp, ${HEADERS_LIST}))

# ---------------------------------- Compilation ----------------------------- #
all: create_dirs ${NAME}

${NAME}: ${OBJS} Makefile
	${C} ${CFLAGS} ${OBJS} ${INCLUDES} -o $@

${OBJS_DIR}%.o: %.cpp ${HEADERS} Makefile | ${OBJS_DIR}
	${C} ${CFLAGS} ${INCLUDES} -c $< -o $@

create_dirs:
	@$(foreach dir, $(sort $(dir $(OBJS))), ${MKDIR} ${dir};)

-include ${DEPS}

# ---------------------------------- Debug ----------------------------------- #
debug: CFLAGS := $(filter-out -Werror, $(CFLAGS))
debug: C = g++
debug: CFLAGS += -DDEBUG -g3
debug: clean create_dirs ${NAME}

# ---------------------------------- Test ----------------------------------- #
test: CFLAGS := $(filter-out -Werror, $(CFLAGS))
test: CFLAGS += -DTEST
test: clean create_dirs ${NAME}


# ---------------------------------- Clean ----------------------------------- #
clean:
	${RMDIR} ${OBJS_DIR}

fclean: clean
	${RM} ${NAME}

re: fclean all

# ---------------------------------- Phony ----------------------------------- #
.PHONY: all clean fclean re

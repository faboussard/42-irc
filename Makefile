#  Copyright 2024 <mbernard>************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/24 21:33:43 by mbernard          #+#    #+#              #
#    Updated: 2024/11/05 10:49:11 by mbernard         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

C = c++
NAME = ircserv
CFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98
MKDIR = mkdir -p
RMDIR = rm -rf

# ---------------------------------- Sources --------------------------------- #
vpath %.cpp src src/commands

HEADERS_LIST = colors Server Config Client Channel Parser numericReplies utils
SRCS = main Server Client Channel Parser Config \
       numericReplies messageManagement utils \
       list pass nick user quit invite mode ping topic

# ---------------------------------- Repertories ----------------------------- #
HEADERS_DIR = includes/
OBJS_DIR = .objs/
OBJS = $(addprefix ${OBJS_DIR}, $(addsuffix .o, ${SRCS}))
HEADERS = $(addprefix ${HEADERS_DIR}, $(addsuffix .hpp, ${HEADERS_LIST}))
INCLUDES = -I ${HEADERS_DIR}
DEPS = ${OBJS:.o=.d}

# ---------------------------------- Compilation ----------------------------- #
all: ${NAME} ${OBJS} | ${OBJS_DIR} Makefile

${NAME}: ${OBJS} Makefile
	${C} ${CFLAGS} ${OBJS} ${INCLUDES} -o $@

${OBJS_DIR}%.o: %.cpp ${HEADERS} Makefile | ${OBJS_DIR}
	${C} ${CFLAGS} ${INCLUDES} -c $< -o $@

-include ${DEPS}

# ---------------------------------- Create Repertory ------------------------ #
${OBJS_DIR}:
			${MKDIR} ${OBJS_DIR}

# ---------------------------------- Debug ----------------------------------- #

debug: CFLAGS += -DDEBUG
debug: fclean $(OBJS_DIR) $(NAME)

# ---------------------------------- Tests ----------------------------------- #

testnumericr: CFLAGS += -DTESTNUMERICR
testnumericr: fclean $(OBJS_DIR) $(NAME)

testlist: CFLAGS += -g3 -DTESTLIST
testlist: fclean $(OBJS_DIR) $(NAME)

# ---------------------------------- Clean ----------------------------------- #
clean:
	${RMDIR} ${OBJS_DIR}

fclean: clean
	${RM} ${NAME}

re:    fclean
	${MAKE} ${NAME}

# ---------------------------------- Phony ----------------------------------- #
.PHONY: all clean fclean re

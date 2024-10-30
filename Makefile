#  Copyright 2024 <faboussa>************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/24 21:33:43 by mbernard          #+#    #+#              #
#    Updated: 2024/10/30 11:16:51 by faboussa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

C = c++
NAME = ircserv
CFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98
MKDIR = mkdir -p
RMDIR = rm -rf

# ---------------------------------- Sources --------------------------------- #
vpath %.cpp src src/commands

HEADERS_LIST = colors Server Client Channel Parser numericReplies utils serverConfig
SRCS = main Server Client Channel Parser numericReplies utils pass nick user invite mode ping topic join unitTests/join

# ---------------------------------- Répertoires ----------------------------- #
HEADERS_DIR = includes/
OBJS_DIR = .objs/
OBJS = $(addprefix ${OBJS_DIR}, $(addsuffix .o, ${SRCS}))
HEADERS = $(addprefix ${HEADERS_DIR}, $(addsuffix .hpp, ${HEADER_LIST}))
INCLUDES = -I ${HEADERS_DIR}
DEPS = ${OBJS:.o=.d}

# ---------------------------------- Compilation ----------------------------- #
all: ${NAME}

${NAME}: ${OBJS} Makefile
	${C} ${CFLAGS} ${OBJS} ${INCLUDES} -o $@

${OBJS_DIR}%.o: %.cpp ${HEADERS} Makefile | ${OBJS_DIR}
	${C} ${CFLAGS} ${INCLUDES} -c $< -o $@

create_dirs:
	@$(foreach dir, $(sort $(dir $(OBJS))), ${MKDIR} ${dir};)

-include ${DEPS}

# ---------------------------------- Debug ----------------------------------- #
debug: CFLAGS := $(filter-out -Werror, $(CFLAGS))
debug: CFLAGS += -DDEBUG
debug: fclean $(NAME)

# ---------------------------------- Test ----------------------------------- #
test: CFLAGS := $(filter-out -Werror, $(CFLAGS))
test: CFLAGS += -DTEST
test: fclean $(NAME)

# ---------------------------------- Clean ----------------------------------- #
clean:
	${RMDIR} ${OBJS_DIR}

fclean: clean
	${RM} ${NAME}

re: fclean all

# ---------------------------------- Phony ----------------------------------- #
.PHONY: all clean fclean re

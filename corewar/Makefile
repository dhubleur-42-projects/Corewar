NAME		=	corewar

SRCS		= 	\
				main.c \
				config.c \
				arguments.c \
				utils.c \
				$(addprefix virtualmachine/, \
					init.c \
					dump.c \
				)

LIBFT_FOLDER	=	../libft
LIBFT		=	$(LIBFT_FOLDER)/libft.a

_OBJS		=	${SRCS:.c=.o}
OBJS		=	$(addprefix build/, $(_OBJS))
OBJS_DEPEND	=	${OBJS:.o=.d}

CC			=	clang
CFLAGS		=	-Wall -Wextra -Werror
INCLUDE		=	-I includes/ -I $(LIBFT_FOLDER)

all		:	$(NAME)

build/%.o	:	srcs/%.c
	@if [ ! -d $(dir $@) ]; then\
		mkdir -p $(dir $@);\
	fi
	$(CC) ${CFLAGS} -MMD -MF $(@:.o=.d) ${INCLUDE} -c $< -o $@

$(LIBFT): FORCE
	@make -C $(LIBFT_FOLDER)

$(NAME)	:	$(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LIBFT)

-include $(OBJS_DEPEND)

clean	:
	rm -Rf build/ $(TEST_NAME)

fclean	:	clean
	rm -f ${NAME}

re		:	fclean
			make ${NAME}

FORCE:

.PHONY	:	all clean fclean re FORCE

NAME		=	codexion

CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -pthread -I. -Icoders

SRCS		=	coders/main.c \
			coders/parser/parse_args.c \
			coders/init/init_coders.c \
			coders/init/init_dongles.c \
			coders/init/init_simulation.c \
			coders/coder/coder_actions.c \
			coders/coder/coder_cycle.c \
			coders/coder/coder_routine.c \
			coders/dongles/dongle_release.c \
			coders/dongles/dongle_take.c \
			coders/heap/heap_pop.c \
			coders/heap/heap_push.c \
			coders/monitor/check_burnout.c \
			coders/monitor/monitor_loop.c \
			coders/scheduler/scheduler.c \
			coders/threads/join_threads.c \
			coders/threads/start_threads.c \
			coders/utils/logger.c \
			coders/utils/sim_stop.c \
			coders/utils/time_utils.c \
			coders/cleanup.c

OBJS		=	$(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

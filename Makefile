NAME		=	codexion

CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -pthread -I. -Icoders

SRCS		=	coders/main.c \
			coders/parser/parse_args.c \
			coders/init/init_simulation.c \
			coders/init/init_runtime.c \
			coders/init/init_coders.c \
			coders/init/init_dongles.c \
			coders/threads/threads_start.c \
			coders/threads/threads_join.c \
			coders/monitor/monitor_routine.c \
			coders/monitor/monitor_burnout.c \
			coders/coder/coder_actions.c \
			coders/coder/coder_cycle.c \
			coders/coder/coder_routine.c \
			coders/dongles/dongle_release.c \
			coders/dongles/dongle_take.c \
			coders/dongles/dongle_wait.c \
			coders/heap/heap_push.c \
			coders/heap/heap_pop.c \
			coders/heap/heap_remove.c \
			coders/scheduler/scheduler.c \
			coders/utils/logger.c \
			coders/utils/parse_numbers.c \
			coders/utils/simulation_stop.c \
			coders/utils/time_utils.c \
			coders/init/cleanup_simulation.c

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

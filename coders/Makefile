NAME		=	codexion

CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -pthread -I.

SRCS		=	main.c \
			parser/parse_args.c \
			init/init_coders.c \
			init/init_dongles.c \
			init/init_simulation.c \
			coder/coder_actions.c \
			coder/coder_cycle.c \
			coder/coder_routine.c \
			dongles/dongle_release.c \
			dongles/dongle_take.c \
			heap/heap_pop.c \
			heap/heap_push.c \
			monitor/check_burnout.c \
			monitor/monitor_loop.c \
			scheduler/scheduler.c \
			threads/join_threads.c \
			threads/start_threads.c \
			utils/logger.c \
			utils/sim_stop.c \
			utils/time_utils.c \
			cleanup.c

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

test: $(NAME)
	./$(NAME) 2 5000 1000 500 500 2 100 fifo

.PHONY: all clean fclean re test

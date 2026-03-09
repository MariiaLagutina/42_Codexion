/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 11:45:24 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/09 17:18:59 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <stdio.h>

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_sim		t_sim;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;
typedef struct s_heap_node	t_heap_node;
typedef struct s_heap		t_heap;

struct s_dongle
{
	int				id;
	int				is_taken;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long			available_at;
	t_heap			*wait_queue;
};

struct s_coder
{
	int				id;
	int				compiles;
	long			last_compile_start;
	pthread_mutex_t	state_mutex;
	pthread_t		thread;
	t_dongle		*left;
	t_dongle		*right;
	t_sim			*sim;
};

struct s_sim
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				required_compiles;
	long			dongle_cooldown;
	t_scheduler		scheduler;
	long			start_time;
	int				stop;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor_thread;
};

struct s_heap_node
{
	t_coder	*coder;
	long	priority;
	long	request_time;
};

struct s_heap
{
	t_heap_node	*nodes;
	int			size;
	int			capacity;
};

long	get_time_ms(void);
void	precise_sleep(t_sim *sim, long duration);
int		simulation_stopped(t_sim *sim);

void	log_action(t_coder *coder, const char *msg);
void	log_death(t_coder *coder);

int		parse_arguments(t_sim *sim, int argc, char **argv);

int		init_simulation(t_sim *sim);
int		init_coders(t_sim *sim);
int		init_dongles(t_sim *sim);

void	cleanup_simulation(t_sim *sim);

int		start_threads(t_sim *sim);
void	wait_threads(t_sim *sim);

void	*coder_routine(void *arg);
void	coder_cycle(t_coder *coder);
void	coder_compile(t_coder *coder);
void	coder_debug(t_coder *coder);
void	coder_refactor(t_coder *coder);

void	take_dongles(t_coder *coder);
void	release_dongles(t_coder *coder);

void	scheduler_push(t_dongle *dongle, t_coder *coder);
t_coder	*scheduler_pop(t_dongle *dongle);

void	heap_push(t_heap *heap, t_coder *coder, long priority);
t_coder	*heap_pop(t_heap *heap);

void	*monitor_routine(void *arg);
int		check_burnout(t_coder *coder);

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_threads.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:26:24 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/23 18:36:57 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** wake_all_dongles - Broadcast on all dongle condition variables
** @sim: Simulation structure with dongle array
**
** Iterates through all dongles and broadcasts on initialized condition variables.
** Used during error recovery to wake any threads blocked in pthread_cond_wait().
** Called from handle_thread_error() to ensure clean shutdown on startup failure.
** Each broadcast wakes all waiters on that dongle, allowing them to check stop flag.
*/
static void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (sim->dongles[i].mutex_init && sim->dongles[i].cond_init)
		{
			pthread_mutex_lock(&sim->dongles[i].mutex);
			pthread_cond_broadcast(&sim->dongles[i].cond);
			pthread_mutex_unlock(&sim->dongles[i].mutex);
		}
		i++;
	}
}

/*
** handle_thread_error - Clean shutdown on thread creation failure
** @sim: Simulation structure
** @created_count: Number of successfully created threads to join
** Returns: 1 (always fails to signal error)
**
** Error recovery sequence:
** 1. Set stop flag to signal all threads to exit
** 2. Wake all condition variables to unblock any sleeping threads
** 3. Join all successfully created threads (with stop flag they'll exit)
**
** Correctness: Without wake_all_dongles(), threads blocked in cond_wait()
** would never wake, causing join() to block indefinitely (deadlock).
** Broadcast ensures all threads see stop=1 and exit gracefully.
**
** Called on pthread_create() failure for any thread.
*/
static int	handle_thread_error(t_sim *sim, int created_count)
{
	int	i;

	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	/*
	** If some coder threads are blocked in cond wait, wake them before join.
	** This avoids a startup-failure path that can block forever.
	** Without this broadcast, threads sleep indefinitely and never exit,
	** causing pthread_join() to hang.
	*/
	wake_all_dongles(sim);
	i = 0;
	while (i < created_count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	return (1);
}

/*
** create_coder_threads - Spawn all coder worker threads
** @sim: Simulation structure
** Returns: 0 on success, 1 on failure (pthread_create error)
**
** Creates number_of_coders threads, each executing coder_routine(&coders[i]).
** On failure mid-creation, calls handle_thread_error() to clean up partial state.
** All threads start paused at monitor loop entry, waiting for simulation to run.
*/
static int	create_coder_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]))
			return (handle_thread_error(sim, i));
		i++;
	}
	return (0);
}

/*
** start_threads - Spawn all worker threads (coders and monitor)
** @sim: Simulation structure with initialized coders and dongles
** Returns: 0 on success, 1 on failure
**
** Creates coder threads first, then monitor thread.
** On any failure, cleans up via handle_thread_error().
** All threads run concurrently after this returns, synchronized by
** sim->start_time set by monitor and condition variables on dongles.
*/
int	start_threads(t_sim *sim)
{
	if (create_coder_threads(sim))
		return (1);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim))
	{
		return (handle_thread_error(sim, sim->number_of_coders));
	}
	return (0);
}


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:52:00 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/23 18:36:57 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** scheduler_push - Enqueue coder for dongle with priority calculation
** @dongle: Dongle resource to wait for
** @coder: Requesting coder thread
** Returns: 0 on success, 1 on failure (nullptr or malloc)
**
** Selects priority based on scheduling algorithm:
** - EDF: priority = last_compile_start + time_to_burnout (earliest deadline first)
** - FIFO: priority = get_time_ms() (first-come, first-served by timestamp)
**
** Synchronization: EDF priority read protected under coder->state_mutex
** to avoid race with coder threads writing last_compile_start.
** FIFO uses current time (atomic read from monotonic clock).
**
** Call context: lock_and_wait() in dongle_take.c, under dongle->mutex lock.
** Stable tie-breaking: request_time assigned in heap_push().
*/
int	scheduler_push(t_dongle *dongle, t_coder *coder)
{
	long	priority;

	if (!dongle || !coder)
		return (1);
	if (coder->sim->scheduler == EDF)
	{
		/*
		** last_compile_start is written by coder threads under state_mutex.
		** Read it under the same mutex to avoid a data race in EDF priority.
		*/
		pthread_mutex_lock(&coder->state_mutex);
		priority = coder->last_compile_start + coder->sim->time_to_burnout;
		pthread_mutex_unlock(&coder->state_mutex);
	}
	else
		priority = get_time_ms();
	if (heap_push(dongle->wait_queue, coder, priority))
		return (1);
	return (0);
}

/*
** scheduler_pop - Dequeue highest priority coder from wait queue
** @dongle: Dongle resource
** Returns: Coder pointer of top priority, NULL if queue empty
**
** Extracts minimum from min-heap (FIFO/EDF priority).
** Called by lock_and_wait() to grant dongle to next waiter.
** Ties broken by request_time (stable FIFO order on priority equality).
*/
t_coder	*scheduler_pop(t_dongle *dongle)
{
	if (!dongle || !dongle->wait_queue || dongle->wait_queue->size == 0)
		return (NULL);
	return (heap_pop(dongle->wait_queue));
}

/*
** scheduler_remove - Safely erase coder from wait queue on abort
** @dongle: Dongle resource
** @coder: Coder to remove (stopped or aborted waiter)
** Returns: 0 on success, 1 if not found or invalid input
**
** Called when coder aborts (simulation_stopped) during wait.
** Prevents orphaned entries that would block other coders.
** Safe to call from within waiter's lock_and_wait() context
** (dongle->mutex already held by caller).
*/
int	scheduler_remove(t_dongle *dongle, t_coder *coder)
{
	if (!dongle || !dongle->wait_queue || !coder)
		return (1);
	return (heap_remove_coder(dongle->wait_queue, coder));
}


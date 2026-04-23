/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_take.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:09:36 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/23 18:36:57 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** wait_cooldown - Sleep until dongle is no longer in cooldown
** @dongle: Dongle resource with cooldown timer
**
** Calculates remaining wait time (available_at - now), then sleeps via
** pthread_cond_timedwait() until cooldown expires. Woken if dongle released
** or coder stops. Protects against negative sleep duration.
** Called from wait_for_turn() under dongle->mutex lock.
*/
static void	wait_cooldown(t_dongle *dongle)
{
	struct timeval	tv;
	struct timespec	ts;
	long			wait_ms;
	long			now;

	now = get_time_ms();
	wait_ms = dongle->available_at - now;
	if (wait_ms <= 0)
		return ;
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + wait_ms / 1000;
	ts.tv_nsec = (tv.tv_usec * 1000) + (wait_ms % 1000) * 1000000;
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}

/*
** wait_for_turn - Block until coder reaches queue top and dongle available
** @dongle: Dongle resource with wait queue (min-heap)
** @coder: Requesting coder thread
** Returns: 1 if acquired, 0 if stopped before acquisition
**
** Spins in while(1) checking three conditions:
** 1. Is coder at queue head? (heap->nodes[0].coder == coder)
**    Yes: check stop, check is_taken, check cooldown
**    No: wait on condition variable for release/stop signals
** 2. If at head but is_taken, wait for release broadcast
** 3. If at head and available, acquire immediately (is_taken = 1)
**
** Abort-safety: Returns 0 immediately on simulation_stopped() check,
** allowing caller to clean up (scheduler_remove from queue).
**
** Called from lock_and_wait() under dongle->mutex lock.
*/
static int	wait_for_turn(t_dongle *dongle, t_coder *coder)
{
	while (1)
	{
		if (dongle->wait_queue->size > 0
			&& dongle->wait_queue->nodes[0].coder == coder)
		{
			if (simulation_stopped(coder->sim))
				return (0);
			if (dongle->is_taken == 0)
			{
				if (get_time_ms() >= dongle->available_at)
				{
					dongle->is_taken = 1;
					return (1);
				}
				wait_cooldown(dongle);
			}
			else
				pthread_cond_wait(&dongle->cond, &dongle->mutex);
		}
		else
		{
			if (simulation_stopped(coder->sim))
				return (0);
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
		}
	}
}

/*
** order_dongles
	- Assign left/right to first/second by ID for deadlock prevention
** @coder: Coder with left and right dongle pointers
** @first: Output pointer to lower-ID dongle
** @second: Output pointer to higher-ID dongle
**
** Ensures all threads acquire dongles in consistent order (ascending ID),
** preventing circular wait chains. Called at start of take_dongles().
*/
static void	order_dongles(t_coder *coder, t_dongle **first, t_dongle **second)
{
	if (coder->left->id < coder->right->id)
	{
		*first = coder->left;
		*second = coder->right;
	}
	else
	{
		*first = coder->right;
		*second = coder->left;
	}
}

/*
** lock_and_wait - Atomically enqueue coder and wait for single dongle
** @coder: Requesting coder thread
** @dongle: Dongle resource to wait for
** Returns: 1 if acquired, 0 on failure or abort
**
** Critical section:
** 1. Lock dongle->mutex
** 2. Enqueue in priority queue (FIFO or EDF) via scheduler_push()
** 3. Wait at top of queue via wait_for_turn()
** 4. On success, pop from queue; on abort, remove from queue
** 5. Unlock dongle->mutex
**
** Heap tie-breaking ensures stable (FIFO) order for coders with equal priority.
** Abort-safety: If stopped while waiting,
	scheduler_remove() cleans queue entry.
** Called by take_dongles() for each dongle acquisition.
*/
static int	lock_and_wait(t_coder *coder, t_dongle *dongle)
{
	int	acquired;

	pthread_mutex_lock(&dongle->mutex);
	if (scheduler_push(dongle, coder))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	acquired = wait_for_turn(dongle, coder);
	if (acquired)
		scheduler_pop(dongle);
	else
		scheduler_remove(dongle, coder);
	pthread_mutex_unlock(&dongle->mutex);
	return (acquired);
}

/*
** release_single_dongle - Mark dongle free and set cooldown timer
** @coder: Releasing coder (provides cooldown duration from sim)
** @dongle: Dongle to release
**
** Sets is_taken = 0, schedules cooldown (available_at = now + cooldown),
** broadcasts all waiters. Called during abort (second dongle fails) to
** rollback first dongle acquisition, or by dongle_release() normally.
** Under dongle->mutex lock (caller responsibility).
*/
static void	release_single_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_taken = 0;
	dongle->available_at = get_time_ms() + coder->sim->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

/*
** take_dongles - Acquire both required dongles in deadlock-free order
** @coder: Requesting coder thread
** Returns: 1 if both acquired, 0 if aborted (simulation stopped)
**
** Algorithm:
** 1. Order dongles by ID (ascending) to prevent deadlock
** 2. Acquire first dongle via lock_and_wait()
** 3. Check for stop (simulation_stopped) between acquisitions
** 4. Acquire second dongle via lock_and_wait()
** 5. If second fails, rollback first (release_single_dongle)
**
** Concurrency guarantees:
** - No deadlock: all threads acquire dongle pairs in same (ID) order
** - Abort-safe: if stop between acquisitions, first is released
** - Atomic from caller's view: coder_compile() gets both or neither
**
** Called from coder_compile() with return check:
**   if (!take_dongles(coder)) return ;  // Acquisition failed, exit
**
** Completion: On success, increments coder->compiles, continues to work.
**            On fail, coder_compile() exits and returns for next cycle.
*/
int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	order_dongles(coder, &first, &second);
	if (!lock_and_wait(coder, first))
		return (0);
	log_action(coder, "has taken a dongle");
	if (!lock_and_wait(coder, second))
	{
		/*
		** Abort case: stop can happen between first and second acquisition.
		** We must release the first dongle immediately; otherwise we can leave
		** a taken resource behind and block other coders indefinitely.
		** This ensures forward progress and no resource leaks.
		*/
		release_single_dongle(coder, first);
		return (0);
	}
	log_action(coder, "has taken a dongle");
	return (1);
}

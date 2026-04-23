/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:35:09 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/23 18:36:57 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** coder_debug - Sleep for debug duration, logging action
** @coder: Coder thread to sleep
**
** Logs "is debugging" and sleeps for time_to_debug milliseconds.
** Called during coder_cycle between state transitions.
*/
void	coder_debug(t_coder *coder)
{
	log_action(coder, "is debugging");
	precise_sleep(coder->sim, coder->sim->time_to_debug);
}

/*
** coder_refactor - Sleep for refactor duration, logging action
** @coder: Coder thread to sleep
**
** Logs "is refactoring" and sleeps for time_to_refactor milliseconds.
** Called during coder_cycle between state transitions.
*/
void	coder_refactor(t_coder *coder)
{
	log_action(coder, "is refactoring");
	precise_sleep(coder->sim, coder->sim->time_to_refactor);
}

/*
** coder_compile - Acquire dongles, increment compile count, work with them
** @coder: Coder thread performing compilation
**
** State machine:
** 1. Early exit if already stopped or compile quota met
** 2. Acquire both dongles in deadlock-free order (take_dongles)
** 3. Check for stop between quitting dongle and working (abort-safe)
** 4. Lock state_mutex, increment compiles,
	record last_compile_start (EDF deadline)
** 5. Sleep (simulating work) then release dongles
**
** Correctness properties:
** - Atomic resource acquisition: take_dongles returns both or nothing
** - Abort-safe: Stop between acquisitions triggers rollback
** - Correct EDF deadline: last_compile_start recorded under state_mutex
** - Compile count only incremented on successful resource acquisition
**
** Call context: Called from coder_cycle() repeatedly until quota reached.
*/
void	coder_compile(t_coder *coder)
{
	if (simulation_stopped(coder->sim))
		return ;
	pthread_mutex_lock(&coder->state_mutex);
	if (coder->compiles >= coder->sim->required_compiles)
	{
		pthread_mutex_unlock(&coder->state_mutex);
		return ;
	}
	pthread_mutex_unlock(&coder->state_mutex);
	/*
	** Acquire both dongles in deadlock-free order.
	** If acquisition fails (e.g., stop triggered), exit early (no work done).
	*/
	if (!take_dongles(coder))
		return ;
	/*
	** Safety check: stop can occur between dongle acquisition and work start.
	** If stop detected, release dongles and exit (abort-safe).
	*/
	if (simulation_stopped(coder->sim))
	{
		release_dongles(coder);
		return ;
	}
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_ms();
	coder->compiles++;
	pthread_mutex_unlock(&coder->state_mutex);
	log_action(coder, "is compiling");
	precise_sleep(coder->sim, coder->sim->time_to_compile);
	release_dongles(coder);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:49:10 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/23 18:37:06 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** get_time_ms - Get elapsed time in milliseconds from monotonic clock
** Returns: Time in milliseconds (rounded down from nanosecond precision)
**
** Uses CLOCK_MONOTONIC (not wall-clock) for robust elapsed timing.
** CLOCK_MONOTONIC is:
**   - Never affected by NTP adjustments or manual system clock changes
**   - Always moves forward (no negative jumps)
**   - Ideal for burnout detection and cooldown timing
**
** Previous implementation used gettimeofday() (wall-clock), which could jump
** backward if admin adjusted system time, breaking burnout logic.
**
** Complexity: O(1) - single syscall to clock_gettime().
** Called frequently: on every wait check, dongle release, compile start.
*/
long	get_time_ms(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

/*
** precise_sleep - Sleep for duration, exiting early if simulation stops
** @sim: Simulation structure with stop flag
** @duration: Sleep duration in milliseconds
**
** Busy-loop approach: repeatedly check elapsed time and stop flag.
** Each iteration sleeps 500 microseconds (usleep), then polls.
** Allows immediate exit on simulation_stopped() without thread wakeup.
**
** Why busy-loop instead of pthread_cond_timedwait()?
** - Simpler (no per-sleep condition variable overhead)
** - Fast exit on stop (no join wait needed)
** - Suitable for short durations (100-1000ms typical)
**
** Accuracy: ±500μs per polling iteration + precision_sleep overhead.
** Abortable: checks stop flag ~every 500μs, exits if true.
*/
void	precise_sleep(t_sim *sim, long duration)
{
	long	start;

	start = get_time_ms();
	while (get_time_ms() - start < duration)
	{
		if (simulation_stopped(sim))
			break ;
		usleep(500);
	}
}

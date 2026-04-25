/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_wait.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:09:36 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/25 14:24:43 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wait_cooldown(t_dongle *dongle)
{
	struct timespec	ts;
	long			wait_ms;
	long			now;

	now = get_time_ms();
	wait_ms = dongle->available_at - now;
	if (wait_ms <= 0)
		return ;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += wait_ms / 1000;
	ts.tv_nsec += (wait_ms % 1000) * 1000000;
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}

int	handle_queue_head(t_dongle *dongle, t_coder *coder)
{
	if (simulation_stopped(coder->sim))
		return (-1);
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
	return (0);
}

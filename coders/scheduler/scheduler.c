/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:52:00 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/25 14:42:35 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	scheduler_push(t_dongle *dongle, t_coder *coder)
{
	long	priority;

	if (!dongle || !coder)
		return (1);
	if (coder->sim->scheduler == EDF)
	{
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

t_coder	*scheduler_pop(t_dongle *dongle)
{
	if (!dongle || !dongle->wait_queue || dongle->wait_queue->size == 0)
		return (NULL);
	return (heap_pop(dongle->wait_queue));
}

int	scheduler_remove(t_dongle *dongle, t_coder *coder)
{
	if (!dongle || !dongle->wait_queue || !coder)
		return (1);
	return (heap_remove_coder(dongle->wait_queue, coder));
}

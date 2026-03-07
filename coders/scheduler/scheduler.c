/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:52:00 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 19:20:50 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	scheduler_push(t_dongle *dongle, t_coder *coder)
{
	long	priority;

	if (!dongle || !coder)
		return ;
	if (coder->sim->scheduler == EDF)
		priority = coder->last_compile_start
			+ coder->sim->time_to_burnout;
	else
		priority = get_time_ms();
	heap_push(dongle->wait_queue, coder, priority);
}

t_coder	*scheduler_pop(t_dongle *dongle)
{
	if (!dongle || !dongle->wait_queue || dongle->wait_queue->size == 0)
		return (NULL);
	return (heap_pop(dongle->wait_queue));
}

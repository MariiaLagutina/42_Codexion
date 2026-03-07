/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:49:33 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 23:04:36 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_action(t_coder *coder, const char *msg)
{
	long	time;

	if (simulation_stopped(coder->sim))
		return ;
	pthread_mutex_lock(&coder->sim->log_mutex);
	if (!simulation_stopped(coder->sim))
	{
		time = get_time_ms() - coder->sim->start_time;
		printf("%ld %d %s\n", time, coder->id, msg);
	}
	pthread_mutex_unlock(&coder->sim->log_mutex);
}

void	log_death(t_coder *coder)
{
	long	time;

	pthread_mutex_lock(&coder->sim->log_mutex);
	time = get_time_ms() - coder->sim->start_time;
	printf("%ld %d burned out\n", time, coder->id);
	pthread_mutex_unlock(&coder->sim->log_mutex);
}

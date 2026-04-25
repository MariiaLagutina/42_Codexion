/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_burnout.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:55:01 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 23:06:56 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_burnout(t_coder *coder)
{
	long	now;
	long	last_compile;
	int		done;

	pthread_mutex_lock(&coder->state_mutex);
	done = (coder->compiles >= coder->sim->required_compiles);
	last_compile = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	if (done)
		return (0);
	now = get_time_ms();
	if (now - last_compile >= coder->sim->time_to_burnout)
	{
		pthread_mutex_lock(&coder->sim->stop_mutex);
		if (coder->sim->stop == 0)
		{
			coder->sim->stop = 1;
			pthread_mutex_unlock(&coder->sim->stop_mutex);
			log_death(coder);
			return (1);
		}
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		return (1);
	}
	return (0);
}

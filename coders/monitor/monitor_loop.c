/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_loop.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:49:57 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/19 21:17:54 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_all_compiled(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		if (sim->coders[i].compiles < sim->required_compiles)
		{
			pthread_mutex_unlock(&sim->coders[i].state_mutex);
			return (0);
		}
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		i++;
	}
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;

	sim = (t_sim *)arg;
	while (!simulation_stopped(sim))
	{
		i = 0;
		while (i < sim->number_of_coders)
		{
			if (check_burnout(&sim->coders[i]))
				return (NULL);
			i++;
		}
		if (check_all_compiled(sim))
		{
			pthread_mutex_lock(&sim->stop_mutex);
			sim->stop = 1;
			pthread_mutex_unlock(&sim->stop_mutex);
			return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}

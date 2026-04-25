/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_start.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:26:24 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/25 14:45:02 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (sim->dongles[i].mutex_init && sim->dongles[i].cond_init)
		{
			pthread_mutex_lock(&sim->dongles[i].mutex);
			pthread_cond_broadcast(&sim->dongles[i].cond);
			pthread_mutex_unlock(&sim->dongles[i].mutex);
		}
		i++;
	}
}

static int	handle_thread_error(t_sim *sim, int created_count)
{
	int	i;

	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	wake_all_dongles(sim);
	i = 0;
	while (i < created_count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	return (1);
}

static int	create_coder_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]))
			return (handle_thread_error(sim, i));
		i++;
	}
	return (0);
}

int	start_threads(t_sim *sim)
{
	if (create_coder_threads(sim))
		return (1);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim))
	{
		return (handle_thread_error(sim, sim->number_of_coders));
	}
	return (0);
}

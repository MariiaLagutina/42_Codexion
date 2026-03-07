/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:12:11 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 23:07:45 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->log_mutex, NULL))
		return (1);
	if (pthread_mutex_init(&sim->stop_mutex, NULL))
	{
		pthread_mutex_destroy(&sim->log_mutex);
		return (1);
	}
	return (0);
}

static void	init_basic_state(t_sim *sim)
{
	sim->stop = 0;
	sim->start_time = get_time_ms();
	sim->coders = NULL;
	sim->dongles = NULL;
}

int	init_simulation(t_sim *sim)
{
	init_basic_state(sim);
	if (init_mutexes(sim))
		return (1);
	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (1);
	}
	memset(sim->coders, 0, sizeof(t_coder) * sim->number_of_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
	{
		free(sim->coders);
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (1);
	}
	memset(sim->dongles, 0, sizeof(t_dongle) * sim->number_of_coders);
	return (0);
}

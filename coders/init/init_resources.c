/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_resources.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:48:20 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/19 18:50:30 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_mutexes(t_sim *sim)
{
	if (sim->log_mutex_init)
		pthread_mutex_destroy(&sim->log_mutex);
	if (sim->stop_mutex_init)
		pthread_mutex_destroy(&sim->stop_mutex);
}

int	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->log_mutex, NULL) == 0)
		sim->log_mutex_init = 1;
	else
		return (1);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) == 0)
		sim->stop_mutex_init = 1;
	else
	{
		destroy_mutexes(sim);
		return (1);
	}
	return (0);
}

static int	init_coders_array(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
		return (1);
	memset(sim->coders, 0, sizeof(t_coder) * sim->number_of_coders);
	return (0);
}

static int	init_dongles_array(t_sim *sim)
{
	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
		return (1);
	memset(sim->dongles, 0, sizeof(t_dongle) * sim->number_of_coders);
	return (0);
}

int	init_arrays(t_sim *sim)
{
	if (init_coders_array(sim))
		return (1);
	if (init_dongles_array(sim))
	{
		free(sim->coders);
		sim->coders = NULL;
		return (1);
	}
	return (0);
}

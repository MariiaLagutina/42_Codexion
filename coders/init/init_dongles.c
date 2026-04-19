/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:13:44 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/19 21:24:03 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongle_mutex(t_dongle *dongle)
{
	if (pthread_mutex_init(&dongle->mutex, NULL))
		return (1);
	dongle->mutex_init = 1;
	if (pthread_cond_init(&dongle->cond, NULL))
	{
		pthread_mutex_destroy(&dongle->mutex);
		dongle->mutex_init = 0;
		return (1);
	}
	dongle->cond_init = 1;
	return (0);
}

static int	init_dongle_state(t_sim *sim, t_dongle *dongle, int id)
{
	int	n;

	n = sim->number_of_coders;
	dongle->id = id;
	dongle->is_taken = 0;
	dongle->available_at = 0;
	dongle->wait_queue = malloc(sizeof(t_heap));
	if (!dongle->wait_queue)
		return (1);
	dongle->wait_queue->nodes = malloc(sizeof(t_heap_node) * n);
	if (!dongle->wait_queue->nodes)
	{
		free(dongle->wait_queue);
		return (1);
	}
	dongle->wait_queue->size = 0;
	dongle->wait_queue->capacity = n;
	return (0);
}

static int	init_single_dongle(t_sim *sim, t_dongle *dongle, int id)
{
	if (init_dongle_state(sim, dongle, id))
		return (1);
	if (init_dongle_mutex(dongle))
	{
		free(dongle->wait_queue->nodes);
		free(dongle->wait_queue);
		return (1);
	}
	return (0);
}

int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (init_single_dongle(sim, &sim->dongles[i], i))
		{
			while (--i >= 0)
			{
				pthread_mutex_destroy(&sim->dongles[i].mutex);
				pthread_cond_destroy(&sim->dongles[i].cond);
				free(sim->dongles[i].wait_queue->nodes);
				free(sim->dongles[i].wait_queue);
				sim->dongles[i].wait_queue = NULL;
			}
			return (1);
		}
		i++;
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:00:00 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 23:09:30 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cleanup_heap(t_heap *heap)
{
	if (!heap)
		return ;
	if (heap->nodes)
		free(heap->nodes);
	free(heap);
}

static void	cleanup_dongles(t_sim *sim)
{
	int	i;

	if (!sim || !sim->dongles)
		return ;
	i = 0;
	while (i < sim->number_of_coders)
	{
		if (sim->dongles[i].wait_queue)
			cleanup_heap(sim->dongles[i].wait_queue);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

static void	cleanup_coders(t_sim *sim)
{
	int	i;

	if (!sim || !sim->coders)
		return ;
	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
}

void	cleanup_simulation(t_sim *sim)
{
	if (!sim)
		return ;
	cleanup_coders(sim);
	cleanup_dongles(sim);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
}

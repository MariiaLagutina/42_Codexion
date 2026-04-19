/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:00:00 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/19 21:22:00 by mlagutin         ###   ########.fr       */
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
		if (sim->dongles[i].mutex_init)
			pthread_mutex_destroy(&sim->dongles[i].mutex);
		if (sim->dongles[i].cond_init)
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
		if (sim->coders[i].mutex_init)
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
	if (sim->log_mutex_init)
		pthread_mutex_destroy(&sim->log_mutex);
	if (sim->stop_mutex_init)
		pthread_mutex_destroy(&sim->stop_mutex);
}
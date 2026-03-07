/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:18:57 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 15:56:17 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_coder_mutex(t_coder *coder)
{
	if (pthread_mutex_init(&coder->state_mutex, NULL))
		return (1);
	return (0);
}

static void	assign_dongles(t_sim *sim, t_coder *coder, int i)
{
	coder->left = &sim->dongles[i];
	coder->right = &sim->dongles[(i + 1) % sim->number_of_coders];
}

int	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compiles = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].sim = sim;
		assign_dongles(sim, &sim->coders[i], i);
		if (init_coder_mutex(&sim->coders[i]))
		{
			while (--i >= 0)
				pthread_mutex_destroy(&sim->coders[i].state_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 12:12:11 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/25 14:05:55 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_basic_state(t_sim *sim)
{
	sim->stop = 0;
	sim->start_time = get_time_ms();
	sim->coders = NULL;
	sim->dongles = NULL;
	sim->log_mutex_init = 0;
	sim->stop_mutex_init = 0;
}

int	init_simulation(t_sim *sim)
{
	init_basic_state(sim);
	if (init_mutexes(sim))
		return (1);
	if (init_arrays(sim))
	{
		destroy_mutexes(sim);
		return (1);
	}
	return (0);
}

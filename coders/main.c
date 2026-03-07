/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 11:57:20 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 15:18:19 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_and_start(t_sim *sim, int argc, char **argv)
{
	if (parse_arguments(sim, argc, argv))
		return (1);
	if (init_simulation(sim))
		return (1);
	if (init_dongles(sim))
		return (1);
	if (init_coders(sim))
		return (1);
	if (start_threads(sim))
		return (1);
	return (0);
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (argc != 9)
	{
		printf("Usage: ./codexion n_coders t_burnout t_compile ");
		printf("t_debug t_refactor n_compiles cooldown scheduler\n");
		return (1);
	}
	memset(&sim, 0, sizeof(t_sim));
	sim.coders = NULL;
	sim.dongles = NULL;
	if (init_and_start(&sim, argc, argv))
	{
		cleanup_simulation(&sim);
		return (1);
	}
	wait_threads(&sim);
	cleanup_simulation(&sim);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 11:57:48 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 19:46:07 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_valid_number(const char *str, int allow_negative)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	if (str[0] == '-')
	{
		if (!allow_negative)
			return (0);
		i = 1;
	}
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	parse_scheduler(char *arg, t_scheduler *scheduler)
{
	if (!strcmp(arg, "fifo"))
		*scheduler = FIFO;
	else if (!strcmp(arg, "edf"))
		*scheduler = EDF;
	else
		return (1);
	return (0);
}

static int	validate_values(t_sim *sim)
{
	if (sim->number_of_coders <= 0)
		return (1);
	if (sim->time_to_burnout <= 0
		|| sim->time_to_compile <= 0
		|| sim->time_to_debug <= 0
		|| sim->time_to_refactor <= 0)
		return (1);
	if (sim->required_compiles < -1 || sim->dongle_cooldown < 0)
		return (1);
	return (0);
}

static int	parse_numbers(t_sim *sim, char **argv)
{
	sim->number_of_coders = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->required_compiles = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	return (0);
}

int	parse_arguments(t_sim *sim, int argc, char **argv)
{
	int	i;

	if (argc != 9)
		return (1);
	i = 1;
	while (i <= 7)
	{
		if (i == 6)
		{
			if (!is_valid_number(argv[i], 1))
				return (1);
		}
		else
		{
			if (!is_valid_number(argv[i], 0))
				return (1);
		}
		i++;
	}
	parse_numbers(sim, argv);
	if (validate_values(sim))
		return (1);
	return (parse_scheduler(argv[8], &sim->scheduler));
}

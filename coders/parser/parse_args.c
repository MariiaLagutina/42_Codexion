/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 11:57:48 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/19 16:05:10 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
	if (sim->required_compiles <= 0 || sim->dongle_cooldown < 0)
		return (1);
	return (0);
}

static int	parse_int_arg(const char *arg, int *out)
{
	long	val;

	if (!ft_strtol(arg, &val))
		return (1);
	if (val > INT_MAX)
		return (1);
	*out = (int)val;
	return (0);
}

static int	parse_numbers(t_sim *sim, char **argv)
{
	if (parse_int_arg(argv[1], &sim->number_of_coders))
		return (1);
	if (!ft_strtol(argv[2], &sim->time_to_burnout))
		return (1);
	if (!ft_strtol(argv[3], &sim->time_to_compile))
		return (1);
	if (!ft_strtol(argv[4], &sim->time_to_debug))
		return (1);
	if (!ft_strtol(argv[5], &sim->time_to_refactor))
		return (1);
	if (parse_int_arg(argv[6], &sim->required_compiles))
		return (1);
	if (!ft_strtol(argv[7], &sim->dongle_cooldown))
		return (1);
	return (0);
}

int	parse_arguments(t_sim *sim, int argc, char **argv)
{
	if (argc != 9)
		return (1);
	if (parse_numbers(sim, argv))
		return (1);
	if (validate_values(sim))
		return (1);
	return (parse_scheduler(argv[8], &sim->scheduler));
}

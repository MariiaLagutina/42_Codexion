/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:35:09 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 22:56:32 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	coder_debug(t_coder *coder)
{
	log_action(coder, "is debugging");
	precise_sleep(coder->sim, coder->sim->time_to_debug);
}

void	coder_refactor(t_coder *coder)
{
	log_action(coder, "is refactoring");
	precise_sleep(coder->sim, coder->sim->time_to_refactor);
}

void	coder_compile(t_coder *coder)
{
	if (simulation_stopped(coder->sim))
		return ;
	pthread_mutex_lock(&coder->state_mutex);
	if (coder->sim->required_compiles != -1
		&& coder->compiles >= coder->sim->required_compiles)
	{
		pthread_mutex_unlock(&coder->state_mutex);
		return ;
	}
	pthread_mutex_unlock(&coder->state_mutex);
	take_dongles(coder);
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_ms();
	coder->compiles++;
	pthread_mutex_unlock(&coder->state_mutex);
	log_action(coder, "is compiling");
	precise_sleep(coder->sim, coder->sim->time_to_compile);
	release_dongles(coder);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_take.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:09:36 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/25 14:25:01 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	order_dongles(t_coder *coder, t_dongle **first, t_dongle **second)
{
	if (coder->left->id < coder->right->id)
	{
		*first = coder->left;
		*second = coder->right;
	}
	else
	{
		*first = coder->right;
		*second = coder->left;
	}
}

static int	wait_for_turn(t_dongle *dongle, t_coder *coder)
{
	int	status;

	while (!simulation_stopped(coder->sim))
	{
		if (dongle->wait_queue->size > 0
			&& dongle->wait_queue->nodes[0].coder == coder)
		{
			status = handle_queue_head(dongle, coder);
			if (status != 0)
				return (status > 0);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	return (0);
}

static int	lock_and_wait(t_coder *coder, t_dongle *dongle)
{
	int	acquired;

	pthread_mutex_lock(&dongle->mutex);
	if (scheduler_push(dongle, coder))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	acquired = wait_for_turn(dongle, coder);
	if (acquired)
		scheduler_pop(dongle);
	else
		scheduler_remove(dongle, coder);
	pthread_mutex_unlock(&dongle->mutex);
	return (acquired);
}

static void	release_single_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_taken = 0;
	dongle->available_at = get_time_ms() + coder->sim->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	order_dongles(coder, &first, &second);
	if (!lock_and_wait(coder, first))
		return (0);
	log_action(coder, "has taken a dongle");
	if (!lock_and_wait(coder, second))
	{
		release_single_dongle(coder, first);
		return (0);
	}
	log_action(coder, "has taken a dongle");
	return (1);
}

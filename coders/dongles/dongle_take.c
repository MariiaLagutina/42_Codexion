/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_take.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 13:09:36 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 22:58:25 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wait_cooldown(t_dongle *dongle)
{
	struct timeval	tv;
	struct timespec	ts;
	long			wait_ms;
	long			now;

	now = get_time_ms();
	wait_ms = dongle->available_at - now;
	if (wait_ms <= 0)
		return ;
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + wait_ms / 1000;
	ts.tv_nsec = (tv.tv_usec * 1000) + (wait_ms % 1000) * 1000000;
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}

static void	wait_for_turn(t_dongle *dongle, t_coder *coder)
{
	while (1)
	{
		if (simulation_stopped(coder->sim))
			break ;
		if (dongle->wait_queue->size > 0
			&& dongle->wait_queue->nodes[0].coder == coder)
		{
			if (dongle->is_taken == 0)
			{
				if (get_time_ms() >= dongle->available_at)
				{
					dongle->is_taken = 1;
					break ;
				}
				wait_cooldown(dongle);
			}
			else
				pthread_cond_wait(&dongle->cond, &dongle->mutex);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
}

static void	order_dongles(t_coder *coder,
				t_dongle **first, t_dongle **second)
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

static void	lock_and_wait(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	scheduler_push(dongle, coder);
	wait_for_turn(dongle, coder);
	scheduler_pop(dongle);
	pthread_mutex_unlock(&dongle->mutex);
}

void	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	order_dongles(coder, &first, &second);
	lock_and_wait(coder, first);
	log_action(coder, "has taken a dongle");
	if (first == second)
	{
		while (!simulation_stopped(coder->sim))
			usleep(1000);
		return ;
	}
	lock_and_wait(coder, second);
	log_action(coder, "has taken a dongle");
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 20:02:31 by mlagutin          #+#    #+#             */
/*   Updated: 2026/02/05 20:02:39 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Инициализируем массив донглов */
int init_dongles(t_rules *rules)
{
	int	i;

	rules->dongle_locks = malloc(sizeof(pthread_mutex_t) * rules->nb_coders);
	if (!rules->dongle_locks)
		return (0);
	i = 0;
	while (i < rules->nb_coders)
	{
		if (pthread_mutex_init(&rules->dongle_locks[i], NULL) != 0)
			return (0);
		i++;
	}
	if (pthread_mutex_init(&rules->log_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&rules->stop_mutex, NULL) != 0)
		return (0);
	return (1);
}

/* Очистка при выходе (чтобы не было утечек) */
void cleanup_all(t_rules *rules, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < rules->nb_coders)
	{
		pthread_mutex_destroy(&rules->dongle_locks[i]);
		i++;
	}
	pthread_mutex_destroy(&rules->log_mutex);
	pthread_mutex_destroy(&rules->stop_mutex);
	if (rules->dongle_locks)
		free(rules->dongle_locks);
	if (coders)
		free(coders);
}

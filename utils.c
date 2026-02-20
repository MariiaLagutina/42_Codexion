/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 19:50:45 by mlagutin          #+#    #+#             */
/*   Updated: 2026/02/05 19:52:54 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

long long get_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000);
}
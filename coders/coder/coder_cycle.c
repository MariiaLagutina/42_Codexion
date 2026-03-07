/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_cycle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 12:34:52 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/06 15:43:20 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	coder_cycle(t_coder *coder)
{
	coder_compile(coder);
	if (simulation_stopped(coder->sim))
		return ;
	coder_debug(coder);
	if (simulation_stopped(coder->sim))
		return ;
	coder_refactor(coder);
}

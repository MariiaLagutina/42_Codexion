/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_remove.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 14:37:53 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/25 14:41:52 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heapify_up_from(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->nodes[parent].priority < heap->nodes[i].priority
			|| (heap->nodes[parent].priority == heap->nodes[i].priority
				&& heap->nodes[parent].request_time
				<= heap->nodes[i].request_time))
			break ;
		swap_nodes(&heap->nodes[parent], &heap->nodes[i]);
		i = parent;
	}
}

int	heap_remove_coder(t_heap *heap, t_coder *coder)
{
	int	i;

	if (!heap || !coder || heap->size == 0)
		return (1);
	i = 0;
	while (i < heap->size && heap->nodes[i].coder != coder)
		i++;
	if (i == heap->size)
		return (1);
	heap->size--;
	if (i == heap->size)
		return (0);
	heap->nodes[i] = heap->nodes[heap->size];
	if (i > 0 && (heap->nodes[i].priority < heap->nodes[(i - 1) / 2].priority
			|| (heap->nodes[i].priority == heap->nodes[(i - 1) / 2].priority
				&& heap->nodes[i].request_time < heap->nodes[(i - 1)
					/ 2].request_time)))
		heapify_up_from(heap, i);
	else
		heapify_down_from(heap, i);
	return (0);
}

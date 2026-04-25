/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_pop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:18:51 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/25 14:38:49 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static int	get_smallest_index(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	smallest;

	left = 2 * i + 1;
	right = 2 * i + 2;
	smallest = i;
	if (left < heap->size)
	{
		if (heap->nodes[left].priority < heap->nodes[smallest].priority
			|| (heap->nodes[left].priority == heap->nodes[smallest].priority
				&& heap->nodes[left].request_time
				< heap->nodes[smallest].request_time))
			smallest = left;
	}
	if (right < heap->size)
	{
		if (heap->nodes[right].priority < heap->nodes[smallest].priority
			|| (heap->nodes[right].priority
				== heap->nodes[smallest].priority
				&& heap->nodes[right].request_time
				< heap->nodes[smallest].request_time))
			smallest = right;
	}
	return (smallest);
}

static void	heapify_down(t_heap *heap)
{
	int	i;
	int	smallest;

	i = 0;
	while (1)
	{
		smallest = get_smallest_index(heap, i);
		if (smallest == i)
			break ;
		swap_nodes(&heap->nodes[i], &heap->nodes[smallest]);
		i = smallest;
	}
}

void	heapify_down_from(t_heap *heap, int i)
{
	int	smallest;

	while (1)
	{
		smallest = get_smallest_index(heap, i);
		if (smallest == i)
			break ;
		swap_nodes(&heap->nodes[i], &heap->nodes[smallest]);
		i = smallest;
	}
}

t_coder	*heap_pop(t_heap *heap)
{
	t_coder	*res;

	if (!heap || heap->size == 0)
		return (NULL);
	res = heap->nodes[0].coder;
	heap->nodes[0] = heap->nodes[--heap->size];
	heapify_down(heap);
	return (res);
}

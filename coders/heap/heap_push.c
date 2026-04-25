/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_push.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:18:12 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/25 14:27:43 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parent_has_priority(t_heap *heap, int parent, int i)
{
	if (heap->nodes[parent].priority < heap->nodes[i].priority)
		return (1);
	if (heap->nodes[parent].priority == heap->nodes[i].priority
		&& heap->nodes[parent].request_time <= heap->nodes[i].request_time)
		return (1);
	return (0);
}

static void	heapify_up(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (parent_has_priority(heap, parent, i))
			break ;
		swap_nodes(&heap->nodes[parent], &heap->nodes[i]);
		i = parent;
	}
}

static int	resize_heap(t_heap *heap)
{
	t_heap_node	*new_nodes;

	if (heap->capacity == 0)
		heap->capacity = 10;
	else
		heap->capacity *= 2;
	new_nodes = malloc(sizeof(t_heap_node) * heap->capacity);
	if (!new_nodes)
		return (1);
	memcpy(new_nodes, heap->nodes, sizeof(t_heap_node) * heap->size);
	free(heap->nodes);
	heap->nodes = new_nodes;
	return (0);
}

int	heap_push(t_heap *heap, t_coder *coder, long priority)
{
	int	i;

	if (!heap || !coder)
		return (1);
	if (heap->size >= heap->capacity)
	{
		if (resize_heap(heap))
			return (1);
	}
	i = heap->size;
	heap->size++;
	heap->nodes[i].coder = coder;
	heap->nodes[i].priority = priority;
	heap->nodes[i].request_time = get_time_ms();
	heapify_up(heap, i);
	return (0);
}

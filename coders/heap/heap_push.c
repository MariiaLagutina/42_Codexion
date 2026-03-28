/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_push.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:18:12 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/28 14:16:38 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	heapify_up(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->nodes[parent].priority <= heap->nodes[i].priority)
			break ;
		swap_nodes(&heap->nodes[parent], &heap->nodes[i]);
		i = parent;
	}
}

static int	resize_heap(t_heap *heap)
{
	t_heap_node	*new_nodes;
	int			i;

	if (heap->capacity == 0)
		heap->capacity = 10;
	else
		heap->capacity *= 2;
	new_nodes = malloc(sizeof(t_heap_node) * heap->capacity);
	if (!new_nodes)
		return (1);
	i = 0;
	while (i < heap->size)
	{
		new_nodes[i] = heap->nodes[i];
		i++;
	}
	free(heap->nodes);
	heap->nodes = new_nodes;
	return (0);
}

void	heap_push(t_heap *heap, t_coder *coder, long priority)
{
	int	i;

	if (!heap || !coder)
		return ;
	if (heap->size >= heap->capacity)
	{
		if (resize_heap(heap))
			return ;
	}
	i = heap->size;
	heap->size++;
	heap->nodes[i].coder = coder;
	heap->nodes[i].priority = priority;
	heapify_up(heap, i);
}

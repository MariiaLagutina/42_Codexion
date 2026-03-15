/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_pop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:18:51 by mlagutin          #+#    #+#             */
/*   Updated: 2026/03/14 14:39:49 by mlagutin         ###   ########.fr       */
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

static int	is_higher_priority(t_heap_node *a, t_heap_node *b)
{
	if (a->priority != b->priority)
		return (a->priority < b->priority);
	return (a->coder->id > b->coder->id);
}

static int	get_smallest_child(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	smallest;

	left = 2 * i + 1;
	right = 2 * i + 2;
	smallest = i;
	if (left < heap->size
		&& is_higher_priority(&heap->nodes[left], &heap->nodes[smallest]))
		smallest = left;
	if (right < heap->size
		&& is_higher_priority(&heap->nodes[right], &heap->nodes[smallest]))
		smallest = right;
	return (smallest);
}

// static int	get_smallest_child(t_heap *heap, int i)
// {
// 	int	left;
// 	int	right;
// 	int	smallest;

// 	left = 2 * i + 1;
// 	right = 2 * i + 2;
// 	smallest = i;
// 	if (left < heap->size
// 		&& heap->nodes[left].priority < heap->nodes[smallest].priority)
// 		smallest = left;
// 	if (right < heap->size
// 		&& heap->nodes[right].priority < heap->nodes[smallest].priority)
// 		smallest = right;
// 	return (smallest);
// }

static void	heapify_down(t_heap *heap)
{
	int	i;
	int	smallest;

	i = 0;
	while (1)
	{
		smallest = get_smallest_child(heap, i);
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

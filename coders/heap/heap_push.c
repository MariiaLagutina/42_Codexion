/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_push.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:18:12 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/23 18:34:55 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** heapify_up - Restore min-heap property by moving element up
** @heap: Min-heap to restore
** @i: Index of element to move up
**
** Compares element with parent and swaps if element is smaller.
** Tie-breaking: if priorities equal, maintain insertion order via request_time.
** (smaller request_time stays above parent in min-heap).
** Called during insertion to restore heap invariant O(log n).
*/
static void	heapify_up(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		/*
		** If parent priority is smaller,
			or equal priority but parent came first,
		** then heap is valid (smaller/earlier should stay higher).
		*/
		if (heap->nodes[parent].priority < heap->nodes[i].priority
			|| (heap->nodes[parent].priority == heap->nodes[i].priority
				&& heap->nodes[parent].request_time <= heap->nodes[i].request_time))
			break ;
		swap_nodes(&heap->nodes[parent], &heap->nodes[i]);
		i = parent;
	}
}

/*
** resize_heap - Double the capacity of min-heap
** @heap: Heap to resize
** Returns: 0 on success, 1 on malloc failure
**
** Allocates new array with double capacity, copies existing nodes, frees old.
** Critical for unbounded queue growth. Protected by caller's dongle mutex.
*/
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

/*
** heap_push - Insert coder into min-heap with stable ordering
** @heap: Min-heap (dongle wait queue)
** @coder: Coder thread requesting resource
** @priority: Priority value (timestamp for FIFO, deadline for EDF)
** Returns: 0 on success, 1 on failure (nullptr or malloc)
**
** Assigns monotonically increasing request_time for stable tie-breaking.
** If two requests have same priority, earlier request gets dongle first.
** Call context: scheduler_push() under dongle mutex lock.
** Example: EDF priority = last_compile_start + time_to_burnout;
**          FIFO priority = get_time_ms() (current time).
** Complexity: O(log n) amortized due to resize.
*/
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
	/*
	** Assign monotonic request_time for stable ordering on tie-break.
	** Protected by caller's dongle->mutex, so no race on g_request_counter++
	*/
	heap->nodes[i].request_time = g_request_counter++;
	heapify_up(heap, i);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_pop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlagutin <mlagutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:18:51 by mlagutin          #+#    #+#             */
/*   Updated: 2026/04/23 18:34:59 by mlagutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** swap_nodes - Exchange two heap nodes in-place
** @a: First node to swap
** @b: Second node to swap
**
** Simple memcpy-style swap via temporary. Used during heapify operations.
*/
void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
** get_smallest_index - Find minimum child (or self) for min-heap
** @heap: Heap structure
** @i: Parent index to check
** Returns: Index of smallest among node[i], node[2*i+1], node[2*i+2]
**
** Compares priorities; on tie, earlier request_time wins (stable ordering).
** Used by heapify_down to restore heap property after removal.
*/
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
				&& heap->nodes[left].request_time < heap->nodes[smallest].request_time))
			smallest = left;
	}
	if (right < heap->size)
	{
		if (heap->nodes[right].priority < heap->nodes[smallest].priority
			|| (heap->nodes[right].priority == heap->nodes[smallest].priority
				&& heap->nodes[right].request_time < heap->nodes[smallest].request_time))
			smallest = right;
	}
	return (smallest);
}

/*
** heapify_down - Restore min-heap from root after removal
** @heap: Heap to restore
**
** Moves root down, swapping with smaller child until heap property restored.
** Called after heap_pop() removes root element. O(log n).
*/
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

/*
** heapify_down_from - Restore min-heap from arbitrary position downward
** @heap: Heap to restore
** @i: Starting index
**
** Like heapify_down but starts at position i (used by heap_remove_coder).
** Moves element down until heap property restored. O(log n).
*/
static void	heapify_down_from(t_heap *heap, int i)
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

/*
** heapify_up_from - Restore min-heap from arbitrary position upward
** @heap: Heap to restore
** @i: Starting index
**
** Moves element up, swapping with parent if element is smaller.
** Tie-breaking: if priorities equal, maintain insertion order via request_time.
** Used by heap_remove_coder when moved element is smaller than parent. O(log n).
*/
static void	heapify_up_from(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		/*
		** If parent priority is smaller or equal with earlier request_time,
		** then heap is valid (min-heap property satisfied).
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
** heap_pop - Extract and remove minimum element (highest priority)
** @heap: Min-heap (dongle wait queue)
** Returns: Coder pointer of top priority waiter, NULL if empty
**
** Removes root (minimum priority), replaces with tail, restores heap.
** Called by scheduler_pop() to grant dongle to next waiting coder.
** Complexity: O(log n).
*/
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

/*
** heap_remove_coder - Erase arbitrary coder from wait queue (abort-safe)
** @heap: Min-heap (dongle wait queue)
** @coder: Coder to remove (usually stopped waiter)
** Returns: 0 on success, 1 if coder not found or heap invalid
**
** Used when coder aborts (simulation_stopped) while waiting for dongle.
** Finds coder, moves tail to hole, reheapifies from position.
** Prevents orphaned queue entries that would block future waiters.
** Complexity: O(n) search + O(log n) reheapify.
**
** Call context: lock_and_wait() in dongle_take.c when abort detected,
** via scheduler_remove() under dongle->mutex lock.
*/
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
	/*
	** After moving tail to removed position, check heap property:
	** if moved element is smaller than parent, bubble up; else bubble down.
	*/
	if (i > 0 && (heap->nodes[i].priority < heap->nodes[(i - 1) / 2].priority
			|| (heap->nodes[i].priority == heap->nodes[(i - 1) / 2].priority
				&& heap->nodes[i].request_time < heap->nodes[(i - 1)
				/ 2].request_time)))
		heapify_up_from(heap, i);
	else
		heapify_down_from(heap, i);
	return (0);
}

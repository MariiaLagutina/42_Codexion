_This project has been created as part of the 42 curriculum by mlagutin._

## Description

**Codexion** is a multi-threaded C simulation of a shared co-working environment where coders compete for limited USB dongles to compile their quantum code. The goal is to orchestrate POSIX threads and synchronize access to shared resources without data races, deadlocks, or coder burnout — using two scheduling algorithms: FIFO and EDF (Earliest Deadline First).

This project is a variation of Dijkstra's classic Dining Philosophers problem, extended with real-time scheduling constraints, dongle cooldowns, and precise burnout detection.

## Instructions

**Compilation:**
```bash
make
```

**Execution:**
```bash
./codexion <n_coders> <t_burnout> <t_compile> <t_debug> <t_refactor> <n_compiles> <cooldown> <scheduler>
```

| Argument | Description |
|---|---|
| `n_coders` | Number of coder threads and dongles |
| `t_burnout` | Max ms before a coder burns out without compiling |
| `t_compile` | Time in ms spent compiling (requires two dongles) |
| `t_debug` | Time in ms spent debugging |
| `t_refactor` | Time in ms spent refactoring |
| `n_compiles` | Required compiles per coder to end simulation safely |
| `cooldown` | Time in ms a dongle is unavailable after release |
| `scheduler` | Scheduling policy: `fifo` or `edf` |

**Examples:**
```bash
./codexion 1 800 200 200 200 1 0 fifo      # single coder edge case
./codexion 3 310 200 200 200 1 0 fifo      # burnout timing case
./codexion 5 800 200 200 200 100 100 edf   # EDF + cooldown stress test
```

## Blocking cases handled

**Deadlock Prevention:**
Coders always acquire dongles in ascending ID order — the dongle with the smaller ID is locked first. This breaks the circular wait condition from Coffman's four necessary conditions for deadlock. Without this ordering, coder N could hold dongle N while waiting for dongle N+1, and coder N+1 could hold dongle N+1 while waiting for dongle N — a classic circular wait.

**Starvation Prevention:**
A custom min-heap priority queue guarantees fair arbitration per dongle. Under FIFO, the request timestamp is the priority — first come, first served. Under EDF, the priority is `last_compile_start + time_to_burnout` — the coder closest to burnout is served first. When priorities are equal, insertion order is preserved by the heap's `<=` comparison in `heapify_up`.

**Cooldown Handling:**
After a coder releases a dongle, it becomes unavailable for `cooldown` milliseconds. This is enforced via `dongle->available_at` timestamp. Waiting coders use `pthread_cond_timedwait` to sleep precisely until the cooldown expires — no busy-waiting, no overshooting.

**Precise Burnout Detection:**
A dedicated monitor thread polls all coders every 0.5ms, checking `now - last_compile_start >= time_to_burnout`. The monitor is the only thread that sets `stop = 1`, protected by `stop_mutex`. Burnout is logged within the required 10ms tolerance. The monitor never holds `stop_mutex` and `log_mutex` simultaneously to avoid lock inversion.

**Log Serialization:**
All output is protected by `log_mutex`. `log_action` uses double-checked locking — checking `simulation_stopped` before and after acquiring the mutex — to avoid printing stale messages after the simulation ends. `log_death` skips this check because the burnout message must always be printed.

## Thread synchronization mechanisms

**`pthread_mutex_t`** is used at four levels:
- `dongle->mutex` — protects each dongle's state and its priority queue. Held during the entire wait cycle so queue operations are atomic.
- `coder->state_mutex` — protects `compiles` and `last_compile_start` from concurrent reads by the monitor and writes by the coder thread.
- `sim->log_mutex` — serializes all printf output so lines never interleave.
- `sim->stop_mutex` — protects the `stop` flag. Used with double-checked locking in `check_burnout` so only the first burnout sets the flag and prints the message.

**`pthread_cond_t`** replaces busy-waiting for dongle access:
- Coders call `pthread_cond_wait` when their turn has not yet come or the dongle is still taken.
- Coders call `pthread_cond_timedwait` when they are first in queue but cooldown has not elapsed.
- `pthread_cond_broadcast` is used on release — not `signal` — because multiple coders may be waiting and each must re-evaluate the condition.
- After the monitor thread joins, a final broadcast on all dongles wakes any coders sleeping in `cond_wait`, allowing clean thread termination.

**Initialization safety:**
Every mutex and condition variable has an `_init` flag. Cleanup only calls `pthread_mutex_destroy` and `pthread_cond_destroy` on primitives that were successfully initialized, preventing undefined behavior on partial initialization failures.

## Resources

- Dijkstra, E.W. (1965) — Dining Philosophers Problem, original formulation of resource sharing and deadlock
- Coffman, E.G. et al. (1971) — "System Deadlocks", four necessary conditions for deadlock
- Liu, C.L. and Layland, J.W. (1973) — "Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment", EDF scheduling theory
- POSIX Threads documentation — [pubs.opengroup.org](https://pubs.opengroup.org)
- Downey, A. — "The Little Book of Semaphores" (free at greenteapress.com)
- `man pthread_cond_wait`, `man pthread_mutex_init`, `man gettimeofday`

## AI Usage:
AI tools were used occasionally to support the learning process, such as finding materials, exploring ideas, and thinking about edge cases.
All core parts of the project were implemented independently.
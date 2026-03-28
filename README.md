_This project has been created as part of the 42 curriculum by mlagutin._

## Description

**Codexion** is a multi-threaded C application simulating a shared co-working environment where coders compete for limited resources (USB dongles) to compile their quantum code. The core objective is to manage POSIX threads and synchronize access to shared resources without causing data races, deadlocks, or coder burnout, using two scheduling algorithms: FIFO and EDF (Earliest Deadline First).

## Instructions

**Compilation:**
To build the simulator, simply run:
`make`

**Execution:**
Run the simulation with the following mandatory arguments:
`./codexion <n_coders> <t_burnout> <t_compile> <t_debug> <t_refactor> <n_compiles> <cooldown> <scheduler>`

- `n_coders`: Number of coder threads (and dongles).
- `t_burnout`: Max time in ms before a coder burns out.
- `t_compile`: Time in ms spent compiling.
- `t_debug`: Time in ms spent debugging.
- `t_refactor`: Time in ms spent refactoring.
- `n_compiles`: Required successful compiles per coder (to end simulation safely).
- `cooldown`: Time in ms a dongle is unavailable after release.
- `scheduler`: Resource scheduling policy (`fifo` or `edf`).

_Example:_ `./codexion 5 800 200 200 200 7 0 fifo`

## Implementation highlights

- Min-Heap based scheduler for FIFO/EDF arbitration.
- Deadlock prevention via ordered dongle locking (lowest ID first).
- Dedicated monitor thread for precise burnout detection.

## Quick tested cases

- `./codexion 1 800 200 200 200 -1 0 fifo` (single coder edge case)
- `./codexion 3 310 200 200 200 -1 0 fifo` (burnout timing case)
- `./codexion 5 800 200 200 200 -1 100 edf` (EDF + cooldown stress)

## Thread synchronization mechanisms

This project utilizes specific threading primitives to ensure robust coordination:

- **`pthread_mutex_t` (Mutual Exclusion):** \* `dongle->mutex`: Strictly protects each dongle's state (`is_taken`, `available_at`, and its priority queue).
  - `coder->state_mutex`: Protects individual coder statistics (`compiles`, `last_compile_start`) from data races when the monitor thread reads them.
  - `sim->log_mutex`: Serializes all logging output so messages never interleave on a single line.
  - `sim->stop_mutex`: Protects the global simulation stop flag.
- **`pthread_cond_t` (Condition Variables):** \* Used extensively to manage the waiting queues for the dongles. Instead of busy-waiting (spinning), coders use `pthread_cond_wait` and `pthread_cond_timedwait` to sleep efficiently until a dongle is released or a cooldown expires. A `pthread_cond_broadcast` wakes them up when a resource becomes available.

## Blocking cases handled

To ensure liveness and prevent the simulation from halting, several concurrency issues were addressed:

- **Deadlock Prevention (Coffman's Conditions):** Implemented a strict hierarchical locking order. Instead of odd/even logic, coders always compare the IDs of their required dongles and attempt to lock the dongle with the _smaller ID_ first. This effectively breaks the circular wait condition.
- **Starvation Prevention:** A custom Min-Heap priority queue guarantees fair arbitration. Under FIFO, arrival time dictates priority. Under EDF, the deadline (`last_compile_start + time_to_burnout`) acts as the priority. A tie-breaker is implemented for EDF to prefer higher coder IDs on equal deadlines.
- **Precise Burnout Detection:** A dedicated monitor thread continuously checks timestamps, ensuring burnouts are logged within the strict 10ms tolerance. To prevent AB-BA deadlocks (inversion of locks), the monitor never holds `stop_mutex` and `log_mutex` simultaneously.
- **Cooldown Handling:** Handled via conditional variables and timestamps (`dongle->available_at`). Coders yield the CPU until the specific cooldown period fully elapses.
- **Log Serialization:** A dedicated global mutex ensures every log line is printed atomically.

## Resources

- **Classic References:** Dijkstra's Philosophers Dining Problem (1965) and Coffman's conditions for deadlock analysis. Real-time scheduling theory for EDF implementation.
- **AI Usage:** AI tools were used occasionally to support the learning process, such as finding materials, exploring ideas, and thinking about edge cases.
All core parts of the project were implemented independently.ns.

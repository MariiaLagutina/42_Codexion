# Codexion — Документация

---

## ASCII-схема проекта

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           ТОЧКА ВХОДА                                       │
│   ./codexion n_coders t_burnout t_compile t_debug t_refactor                │
│               n_compiles cooldown scheduler(fifo|edf)                       │
└────────────────────────────┬────────────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  main.c :: main()                                                           │
│  ┌──────────────────────────────────────────────────────────────────────┐   │
│  │  memset(&sim, 0 ...)   →  init_and_start(&sim, argc, argv)           │   │
│  └──────────────────────────────────────────────────────────────────────┘   │
└────────────────────────────┬────────────────────────────────────────────────┘
                             │
              ┌──────────────┼──────────────────────┐
              ▼              ▼                      ▼
     parse_arguments()  init_simulation()    init_dongles()
     parser/            init/                init/
     parse_args.c       init_simulation.c    init_dongles.c
     │                  │                   │
     │  Читает 8 арг.   │  malloc coders[]  │  malloc wait_queue (heap)
     │  проверяет       │  malloc dongles[] │  pthread_mutex_init
     │  типы и диапазон │  log_mutex        │  pthread_cond_init
     │  fifo / edf      │  stop_mutex       │  available_at = 0
     └──────────────────┴───────────────────┘
                             │
                             ▼
                      init_coders()
                      init/init_coders.c
                      │
                      │  coder[i].id = i+1
                      │  coder[i].left  = &dongles[i]
                      │  coder[i].right = &dongles[(i+1) % N]
                      │  state_mutex
                      │
                      │     Топология «кольцо»:
                      │
                      │  coder[0]─dongle[0]─coder[1]─dongle[1]─coder[2]
                      │     └────────────────dongle[N-1]───────────────┘
                      │
                      ▼
               start_threads()
               threads/start_threads.c
               │
               ├── pthread_create × N  →  coder_routine()
               └── pthread_create × 1  →  monitor_routine()
                         │
           ┌─────────────┴─────────────────────────────┐
           │                                           │
           ▼                                           ▼
  ╔══════════════════════════╗             ╔══════════════════════════╗
  ║  ПОТОК КОДЕРА (× N)      ║             ║  ПОТОК МОНИТОРА (× 1)    ║
  ║  coder/coder_routine.c   ║             ║  monitor/monitor_loop.c  ║
  ║                          ║             ║                          ║
  ║  if (id % 2 == 0)        ║             ║  каждые 500 мкс:         ║
  ║    usleep(1000)  ← чтобы ║             ║  ┌──────────────────────┐║
  ║    четные стартовали     ║             ║  │ check_burnout()      │║
  ║    чуть позже            ║             ║  │ для каждого кодера   │║
  ║                          ║             ║  │                      │║
  ║  loop:                   ║             ║  │ now - last_compile   │║
  ║    coder_cycle()  ───────╫─┐           ║  │ >= time_to_burnout ? │║
  ║  until stop == 1         ║ │           ║  │   → stop=1, log_death│║
  ╚══════════════════════════╝ │           ║  └──────────────────────┘║
                               │           ║  ┌──────────────────────┐║
                               │           ║  │ check_all_compiled() │║
                               │           ║  │ все скомпилировали   │║
                               │           ║  │ required_compiles?   │║
                               │           ║  │   → stop=1           │║
                               │           ║  └──────────────────────┘║
                               │           ╚══════════════════════════╝
                               │
                               ▼
              ┌────────────────────────────────────┐
              │        coder_cycle()               │
              │        coder/coder_cycle.c         │
              │                                    │
              │  1. coder_compile()  ←─┐           │
              │  2. coder_debug()      │ повтор    │
              │  3. coder_refactor() ──┘           │
              └──────┬─────────────────────────────┘
                     │
         ┌───────────┴────────────────────────────────┐
         │                                            │
         ▼                                            ▼
  coder_compile()                          coder_debug() / coder_refactor()
  coder/coder_actions.c                    coder/coder_actions.c
  │                                        │
  │  1. compiles >= required? → return     │  log_action("is debugging")
  │  2. take_dongles() ◄──────────────┐   │  precise_sleep(time_to_debug)
  │  3. last_compile_start = now       │   │
  │  4. compiles++                     │   │  log_action("is refactoring")
  │  5. log_action("is compiling")     │   │  precise_sleep(time_to_refactor)
  │  6. precise_sleep(time_to_compile) │   └──────────────────────────────
  │  7. release_dongles() ─────────────┘
  │
  └─────────────────────────────────────────────────────────────────
  
  ЗАХВАТ ДОНГЛОВ (take_dongles):
  dongles/dongle_take.c
  
    order_dongles()  → берём сначала донгл с меньшим id
                       (защита от дедлока)
    для каждого донгла:
    ┌─────────────────────────────────────────────────────────────┐
    │  lock(dongle->mutex)                                        │
    │  scheduler_push() → добавляем кодера в очередь ожидания     │
    │                                                             │
    │        FIFO: priority = текущее время                       │
    │        EDF:  priority = last_compile + time_to_burnout      │
    │              (кто раньше сгорит — тот первый)               │
    │                                                             │
    │  wait_for_turn():                                           │
    │    пока кодер не первый в очереди                           │
    │      ИЛИ донгл занят                                        │
    │      ИЛИ cooldown не истёк:                                 │
    │        pthread_cond_wait() / timedwait()                    │
    │  → dongle->is_taken = 1                                     │
    │  unlock(dongle->mutex)                                      │
    └─────────────────────────────────────────────────────────────┘
  
  ОЧЕРЕДЬ ОЖИДАНИЯ (min-heap):
  scheduler/scheduler.c + heap/heap_push.c + heap/heap_pop.c
  
    heap_push() → добавляет кодера с приоритетом (меньше = важнее)
    heap_pop()  → извлекает кодера с наименьшим приоритетом
    
  ОСВОБОЖДЕНИЕ ДОНГЛОВ (release_dongles):
  dongles/dongle_release.c
  
    is_taken = 0
    available_at = now + dongle_cooldown
    pthread_cond_broadcast() → будим всех ждущих
  
  ОСТАНОВКА И ОЧИСТКА:
  
    wait_threads()                 cleanup_simulation()
    threads/join_threads.c         cleanup.c
    │                              │
    │  join(monitor_thread)        │  free(coders)
    │  broadcast все донглы        │  free(dongles)
    │    (разбудить всех)          │  destroy mutexes/conds
    │  join(coder_threads × N)     │  free wait_queues
```

---

## Флоу проекта — простой русский

### Что это вообще такое?

Это симуляция «задачи обедающих философов», только вместо философов с вилками — **программисты с USB-донглами**. Каждому кодеру нужны два донгла (левый и правый), чтобы запустить компиляцию. Если кодер слишком долго не может откомпилировать — он сгорает (burnout).

---

### Шаг 1 — Запуск программы

Программа запускается с 8 аргументами:

```
./codexion  5  800  200  100  150  3  50  edf
             │   │    │    │    │  │   │   │
             │   │    │    │    │  │   │   └── алгоритм планировщика (fifo или edf)
             │   │    │    │    │  │   └────── cooldown между компиляциями (мс)
             │   │    │    │    │  └────────── сколько раз нужно скомпилировать
             │   │    │    │    └───────────── время рефакторинга (мс)
             │   │    │    └────────────────── время дебага (мс)
             │   │    └─────────────────────── время компиляции (мс)
             │   └──────────────────────────── через сколько мс наступает burnout
             └──────────────────────────────── количество кодеров
```

Если аргументов не 9 (включая имя программы) — выводится подсказка и выход.

---

### Шаг 2 — Парсинг и валидация

Функция `parse_arguments()` проверяет:
- все числа — это действительно числа
- все значения положительные (cooldown может быть 0)
- планировщик — это `fifo` или `edf`

Всё сохраняется в структуру `t_sim`.

---

### Шаг 3 — Инициализация

Три последовательных шага:

1. **`init_simulation()`** — создаёт глобальные мьютексы (`log_mutex` для вывода, `stop_mutex` для флага остановки), выделяет память под массивы кодеров и донглов, фиксирует `start_time`.

2. **`init_dongles()`** — для каждого донгла создаётся мьютекс, condition variable и пустая куча-очередь (`wait_queue`). У каждого донгла есть флаг `is_taken` и метка `available_at` (когда донгл снова доступен после cooldown).

3. **`init_coders()`** — каждому кодеру назначается id (1-based), счётчик компиляций, ссылка на симуляцию и **два донгла по кругу**: левый — донгл `i`, правый — донгл `(i+1) % N`. Последний кодер замыкает кольцо.

---

### Шаг 4 — Запуск потоков

`start_threads()` создаёт:
- **N потоков кодеров** — каждый запускает `coder_routine()`
- **1 поток монитора** — запускает `monitor_routine()`

Чётные кодеры (id % 2 == 0) засыпают на 1 мс в начале — чтобы нечётные успели взять донглы первыми и все не заблокировались одновременно.

---

### Шаг 5 — Жизнь кодера (главный цикл)

Каждый кодер в бесконечном цикле делает одно и то же:

```
пока симуляция не остановлена:
    1. КОМПИЛЯЦИЯ  — захватить оба донгла, залечь на t_compile мс, отпустить
    2. ДЕБАГ       — просто спать t_debug мс
    3. РЕФАКТОРИНГ — просто спать t_refactor мс
```

Между циклами кодер проверяет флаг `stop` — если он выставлен, цикл прерывается.

---

### Шаг 6 — Захват донглов (самое сложное)

Перед компиляцией кодер должен взять **оба** донгла. Чтобы не было дедлока (когда все взяли по одному и ждут второй), донглы **всегда берутся в порядке возрастания id**.

Для каждого донгла:
1. Кодер встаёт в **очередь ожидания** (`scheduler_push`). Очередь реализована как min-heap, поэтому порядок зависит от планировщика:
   - **FIFO**: приоритет = текущее время → кто первый встал, тот первый и получит
   - **EDF** (Earliest Deadline First): приоритет = время следующего burnout → кто ближе к сгоранию, получит донгл раньше
2. Кодер ждёт через `pthread_cond_wait()`, пока не выполнятся три условия одновременно:
   - Он первый в очереди
   - Донгл свободен (`is_taken == 0`)
   - Cooldown после прошлой компиляции истёк
3. Как только все условия выполнены — кодер помечает донгл занятым и идёт дальше.

---

### Шаг 7 — Компиляция и освобождение

Взяв оба донгла, кодер:
- Запоминает время начала (`last_compile_start = now`) — это сбрасывает таймер burnout
- Увеличивает счётчик `compiles++`
- Выводит в лог `"is compiling"`
- Спит `time_to_compile` мс
- Освобождает оба донгла: `is_taken = 0`, выставляет `available_at = now + cooldown`, будит всех ожидающих через `broadcast`

---

### Шаг 8 — Монитор

Параллельно с кодерами монитор крутится в цикле с задержкой 500 мкс и проверяет два условия:

**Условие смерти (burnout):**
```
текущее время - last_compile_start >= time_to_burnout
```
Если кодер не компилировал слишком долго — выставляется `stop = 1`, выводится `"burned out"` и монитор завершается.

**Условие победы:**
```
все кодеры сделали >= required_compiles компиляций
```
Выставляется `stop = 1`, и монитор завершается.

---

### Шаг 9 — Завершение

Когда `stop = 1`:
1. Монитор завершается первым.
2. `wait_threads()` делает `join` на поток монитора.
3. Затем делает `broadcast` на все донглы — это будит всех заснувших кодеров, чтобы они увидели флаг `stop` и тоже завершились.
4. Делает `join` на все потоки кодеров.
5. `cleanup_simulation()` освобождает всю память и уничтожает мьютексы.

---

### Вывод в консоль

Каждое событие печатается в формате:
```
<время_мс> <id_кодера> <действие>
```
Например:
```
0    1 is compiling
0    3 is compiling
201  1 is debugging
201  3 is debugging
```
Вывод защищён `log_mutex` — все строки атомарны и не перемешиваются.

---

---

## Project Flow — Simple English

### What is this?

This is a simulation of the **Dining Philosophers problem**, but instead of philosophers and forks we have **coders and USB dongles**. Each coder needs two dongles (left and right) to compile code. If a coder goes too long without compiling, they burn out and the simulation ends.

---

### Step 1 — Running the program

The program takes 8 arguments:

```
./codexion  5  800  200  100  150  3  50  edf
             │   │    │    │    │  │   │   │
             │   │    │    │    │  │   │   └── scheduler algorithm (fifo or edf)
             │   │    │    │    │  │   └────── cooldown between compilations (ms)
             │   │    │    │    │  └────────── how many times each coder must compile
             │   │    │    │    └───────────── time to refactor (ms)
             │   │    │    └────────────────── time to debug (ms)
             │   │    └─────────────────────── time to compile (ms)
             │   └──────────────────────────── ms before burnout
             └──────────────────────────────── number of coders
```

If the argument count is wrong, a usage hint is printed and the program exits.

---

### Step 2 — Parsing and validation

`parse_arguments()` checks that:
- every value is actually a number
- all values are positive (cooldown may be 0)
- the scheduler is either `fifo` or `edf`

Everything is stored in the `t_sim` structure.

---

### Step 3 — Initialization

Three sequential steps:

1. **`init_simulation()`** — initializes global mutexes (`log_mutex` for output, `stop_mutex` for the stop flag), allocates memory for coder and dongle arrays, records `start_time`.

2. **`init_dongles()`** — for each dongle: creates a mutex, a condition variable, and an empty min-heap wait queue. Each dongle tracks `is_taken` and `available_at` (when it becomes usable again after cooldown).

3. **`init_coders()`** — assigns each coder an id (1-based), a compile counter, a pointer back to the simulation, and **two dongles in a circular layout**: left = dongle `i`, right = dongle `(i+1) % N`. The last coder wraps around and shares a dongle with the first.

---

### Step 4 — Starting threads

`start_threads()` creates:
- **N coder threads** — each runs `coder_routine()`
- **1 monitor thread** — runs `monitor_routine()`

Even-id coders (id % 2 == 0) sleep for 1 ms at start so odd coders can grab dongles first and avoid an immediate deadlock.

---

### Step 5 — Coder life (main loop)

Each coder repeatedly does the same three things:

```
while simulation is not stopped:
    1. COMPILE   — grab both dongles, sleep t_compile ms, release them
    2. DEBUG     — just sleep t_debug ms
    3. REFACTOR  — just sleep t_refactor ms
```

At every step the coder checks the `stop` flag and exits the loop if it is set.

---

### Step 6 — Taking dongles (the tricky part)

Before compiling, a coder must grab **both** dongles. To prevent deadlock (where every coder holds one dongle and waits for the other), dongles are **always acquired in ascending id order**.

For each dongle:
1. The coder enters the **wait queue** via `scheduler_push()`. The queue is a min-heap, so priority depends on the scheduler:
   - **FIFO**: priority = current timestamp → first come, first served
   - **EDF** (Earliest Deadline First): priority = time until next burnout → whoever is closest to burning out gets the dongle first
2. The coder blocks on `pthread_cond_wait()` until all three conditions are true at the same time:
   - The coder is first in the queue
   - The dongle is free (`is_taken == 0`)
   - The cooldown since the last release has elapsed
3. Once all conditions are met, the coder marks the dongle as taken and continues.

---

### Step 7 — Compiling and releasing

With both dongles held, the coder:
- Records `last_compile_start = now` — this resets the burnout timer
- Increments `compiles++`
- Logs `"is compiling"`
- Sleeps for `time_to_compile` ms
- Releases both dongles: sets `is_taken = 0`, sets `available_at = now + cooldown`, and wakes all waiting coders via `broadcast`

---

### Step 8 — The monitor

Running alongside the coders, the monitor loops every 500 µs and checks two conditions:

**Death condition (burnout):**
```
current time - last_compile_start >= time_to_burnout
```
If any coder went too long without compiling, `stop = 1` is set, `"burned out"` is printed, and the monitor exits.

**Success condition:**
```
every coder has compiled >= required_compiles times
```
`stop = 1` is set and the monitor exits.

---

### Step 9 — Shutdown

When `stop = 1`:
1. The monitor thread finishes first.
2. `wait_threads()` joins the monitor thread.
3. It then broadcasts on all dongle condition variables — this wakes any coder that is sleeping inside `cond_wait`, so it can see `stop == 1` and exit cleanly.
4. All coder threads are joined.
5. `cleanup_simulation()` frees all memory and destroys all mutexes and condition variables.

---

### Console output format

Every event is printed as:
```
<timestamp_ms> <coder_id> <action>
```
Example:
```
0    1 is compiling
0    3 is compiling
201  1 is debugging
201  3 is debugging
350  2 burned out
```
Output is protected by `log_mutex` — every line is printed atomically so lines never interleave.

# [Operating System] Synchronization

I'll admit it. I am in a relationship–quite an intense one I'd say-with this book, right, this BOOK named [Operating System Concepts](http://os-book.com/OS9/index.html). It started as a curious fling, yet quickly evolved to a full-fledge devotion. Now almost all my weekends are gone, sometimes weekdays even. And there is no backing down once you begin.

So here are a couple of reasons why I am disclosing (a piece of) my personal life here:
1. Now that it is public, people will probably be more forgiving in my frequent disappearing on weekends.
2. There is no good hiding this great book and enjoying it all by myself anyway.
3. I decided to do a series on the concepts I learned from it.

As the title suggests, this article is about synchronization in operating systems. As we may have already suspected, this is not the first chapter. This is chapter six, to be exact. Why do I start at chapter six? Well because I just finished this chapter, obviously. Why postponed it to some six weeks later when my memory already becomes stale and unreliable? But since I call it a series, I do plan to introduce previous chapters as well. They will be there.

Anyway, I talked too much. Without furthur ado, let's dive in.

Caution: This post is more of a note than an article (for now). I am still trying to figure out how to present the materials in the most reader-friendly way. Please bear with me until I do.

### Content
01. Some Background
    - Concurrency
    - Race Condition
02. The Critical Section Problem
03. Three Requirement for a Valid Solution
    - Mutual Exclusion
    - Progress
    - Bounded Waiting
04. Peterson's Soluton: an example
05. Hardware-based Solutions
    - `test_and_set()` instruction
    - `compare_and_swap()` instruction
06. Software-based Solutions
    - Mutex Locks
    - Semaphores
07. Implementation Obstacles
    - Deadlocks
    - Starvation
    - Priority Inversion
08. Classic Problems of Synchronization(the three tests to pass)
    - The Bounded-Buffer Problem
    - The Reader-Writer Problem
    - The Dining Philosophers Problem
09. Monitors: The High-Level Synchronization Construct
   - The condition Construct
   - Example: Solving The Dining Philosophers Problem
   - Resource Allocator
   - Java Monitors
10. [OS Synchronization Example] Windows
11. [OS Synchronization Example] Linux
    - Preemptive Kernel vs Nonpreemptive Kernel
12. [OS Synchronization Example] Solaris
13. [User-Level Synchronization Example] Pthreads

Okay. It does seem a lot. Have faith. We will make it.

### 01 Some Background
Before we get to the meat, let us first make sure that we all undersatnd these concepts.
- Processes can execute [in parallel](https://en.wikipedia.org/wiki/Parallel_computing) or  [coucurrently](https://en.wikipedia.org/wiki/Concurrency_(computer_science)).
  - Concurrent execution means that one process may only partially complete execution before the CPU is give to another process.
  - Prallel execution means 1+ processes can run simultaneously on separate processing cores.
- When they do so, they are risking **data inconsistency** if they access a shared data at the same time, thus impact the integrity of data.
- [Race Condition](https://en.wikipedia.org/wiki/Race_condition) is a typical situation that causes data inconsistency.
- How does race condition occur?
  1. Several processes access and manipulate the same data concurrently.
  2. The outcome of the execution depends on the particulat order.
- How to avoid race condition?
  - Ensure that only one process at a time can be manipulating the shared data.

### 02 The Critical Section Problem
- Critical section is a segment of code in a process. The system ensures that when a process is executing in its critical section, no other processes can execute in their critical section. That is, no two processes are executing in their critical sections at the same time.
- The critical section problems is to design a protocol that the processes can use to cooperate.
- The general structure of a typical process P:

  ```c
  do {
    // entry section
    // critical section
    // exit section
    // remainder section
  } while(true);
  ```

### 03 Three Requirements for a Valid Solution
Any valid solution would need to meet the following three requirement:

#### Mutual Exclusion
> If process P is executing in its critical section, then no other processes can be executing in their critical sections.

#### Progress
> If no process is executing in its critical section and some processes wish to enter their critical sections, then only those processes that are not executing in their remainder sections can participate in deciding which will enter its critical section next, and this selection cannot be postpoed indfinitely.

That is, when there is an availability, a process must be selected, and the process cannot be executing in its remainder section.

#### Bounded Waiting
> There exist a bound, or limit, on the number of times that other processes are allowed to enter their critical sections after as process has made a request to enter its critical section and before that request is granted.

This one is the hardest to meet. It requires that we need to guarantee that no process should wait indefinitely(we call it starvation) for permission to execute its critical section.

### 04 Peterson's Soluton: an example
- Peterson's solution is a classic software-based solution to the critical-sectino problem.
- But note that there are no guarantees that Peterson's solution will work correctly on modern computer architectures, because of the way they prerform basic machine-language instructions such as `load` and `store`.
- This solution is restricted to **two** processes that alternate execution between their critical sections and remainder sections.
- Data Items that the processes share:
  - `int turn`  --> whose turn it is to enter its critical section
  - `boolean flag[2]`  --> indicate if a process is ready to enter its critical section
- The structure of process Pi in Peterson's solution(assuming there are process Pi and Pj):

  ```c
  do {
    flag[i] = true;  // entry section
    turn = j;
    while (flag[j] && turn == j);

    // critical section

    flag[i] = false; // exit section

    // remainder section
  } while(true);
  ```

- Steps(for process Pi):
  1. Pi sets `flag[i]` to be true and `turn` to j --> if the other process wishes to enter its critical section, it can do so.
  2. If both processes execute at the same time, only one assignment will last(the other will be overwritten).
  3. The eventual value of turn determines which process will enter its critical section first.
  4. If the other process is executing its critical section (i.e. `turn` == j) Pi will wait.
  5. When the waiting (the `while` part) is over, Pi enters its critical section.
  6. After execution Pi sets its flag to false, signaling it is out of its critical section.
  7. Pi executes in the remainder section.
- Does this solution meet all the requirements? Yes.

### 05 Hardware-based Solutions
- Some simple hardware instructions that are available on many systems.
- Hardware features can make any programming task easier and imporve system efficiency.
- In a **single-processor environment**, we just need to prevent interrupts while a shared variable was being modified.
  --> so the current sequence of instruction can execute in order without preemption.
- In a **multiprocessor environment**, however, disabling interrupts is not a good choice because
  1. It is time consuming, because the message is passed to all processors.
  2. It impacts efficiency, because message passing delays entry into each critical section.
- Many modern computer systems provide special **harware instructions** that allow us to do some tasks automically(as one uninterruptible unit), as follows:
  1. test and modify the content of a word --> `test_and_set()`
    - importance: if the machine supports this instruction, then we can implement mutual exclusion by declaring a boolean variable `lock` and initialize it to `false` as shown below.
    - Definition:
    ```c
    boolean test_and_set(boolean *target) {
      boolean rv = *target;
      *target = true;

      return rv;
    }
    ```

    - Mutual-Exclusion Implementation with `test_and_set()` instruction:
      (note: this implementation does not satisfy the bounded-waiting requirement)
    ```c
    do {
      while (test_and_set(&lock))
        ; // do nothing

      // critical section

      lock = false;

      // remainder section
    } while (true);
    ```

    - Bounded-Waiting Mutual Exclusion with `test_and_set()`:
    ```c
    // the common data structure:
    boolean waiting[n];
    boolean lock;

    do {
      waiting[i] = true;
      key = true;
      while (waiting[i] && key)
        key = test_and_set(&lock);
      waiting[i] = false;

      // critical section

      j = (i + 1) % n;
      while((j != i) && !waiting[j])
        j = (j = 1) % n;

      if (j == i)
        lock = false;
      else
        waiting[j] = false;

      // remainder section
    } while (true);
    ```

  2. swap the contents of two words --> `compare_and_swap()`
    - Always returns the original value of the variable `value`.
    - Provides mutual exclusion:
      1. Declare and initialize a global variable(`lock`) to 0.
      2. The first process, P1, invokes `compare_and_swap()` and sets `lock` to 1.
      3. P1 enters critical section, because the original value of `lock` was equal to the expected value of 0.
      4. Meanwhile, other processes are prevented from entering their critical section because `lock` is not 0 now.
      5. P1 exits critical section, sets `lock` back to 0.
      6. Another process enters its critical section.
    - Definition
    ```c
    int compare_and_swap(int *value, int expected, int new_value) {
      int temp = *value;

      if (*value == expected)
        *value = new_value;

      return temp;
    }
    ```
    - Mutual-Exclusion implementation with `compare_and_swap()` instruction:
      (note: this implementation does not satisfy the bounded-waiting requirement)

    ```c
    do {
      while (compare_and_swap(&lock, 0, 1) != 0)
        ; // do nothing

      // critical section

      lock = 0;

      // remainder section
    } while (true);
    ```

### 06 Software-based Solutions
- The hardware-based solutions are complicated as well as generally inaccessible to application programmers.
- So operating-systems designers build software tools to solve the critical section problem.
- Generally there are two kinds of the tools:
  - mutex lock (mutex = mutual exclusion)
  - semaphore

#### Mutex Locks
- The process must `acquire()` the lock before entering a critical section, and `release()` the lock on exiting the critical section.
  - structure:
  ```c
  do {
    // acquire lock

    // critical section

    // release lock

    // remainder section
  } while (true)
  ```

- A mutex lock uses a boolean variable `available` to indicate if the lock is available or not.
- Definition:
  ```c
  acquire() {
    while (!available)
      ; // busy wait
    available = false;
  }

  release() {
    available = true;
  }
  ```

- Calls to `acquire()` or `release()` must be performed atomically. So mutex locks are often implemented with on of the hardware mechanisms described above.

#### Spinlocks: The Busy Waiting Mutex Lock
- While a process is in its critical section, any other process that tries ot enter its critical section must loop continuously (in its call to `acquire()`).
- The process 'spins' while waiting for the lock to become available.
- Disadvantage: For a real multiprogramming system(where a single CPU is shared among many rocesses), busy waiting wastes CUP cycles.
- Advangate: no context switch is required.

#### Semaphores
- Behaves simiparly to a mutex lock but more sophisticated for process synchronization.
- A **semaphore** S is an integer variable that is accessed only through two standard atomic opeartions(apart from initialization): `wait()` and `signal()`.
  - Definitions:
  ```c
  wait(S) {
    while (S <= 0)
      ; //busy wait
    S--;
  }

  signal(S) {
    S++;
  }
  ```

- When one process midifies the semaphore value, no other process can simultaneously modify that same semaphore value.
- binary vs counting semaphors
  - [binary semaphore] values can range between 0 and 1.
    - behaves like a mutex lock.
  - [counting semaphor] values can range over an unrestricted doain.
    - useful to control access to a given resource consisting of a finite number of instances.
    - The semaphore is initialized to the number of resources available.
    - If a process wishes to use a resource, it performs a `wait()`–decrementing the count.
    - When the process releases the resource, it performs a `signal()`–incrementing the count.
- Implementation without Busy Waiting
  - Make the process **block itself** instead of busy waiting.
  - The block operation places a process into a waiting queue associated with the semaphores, and the state of the process is switched to the waiting state.
  - The put-to-wait process should be restarted (`wakeup()`) and put back to the ready queue  when someone issues  a `signal()` operation.
  - Structure of a Semaphore Implementation
  ```c
  typedef struct {
    int value;
    struct process *list;
  } semaphore;

  wait(semaphore *S) {
    S->value--;
    if (S->value < 0) {
      // add this process to S->list
      ...
      block();
    }
  }

  signal(semaphore *S) {
    S->value++;
    if (S->value <=0 ) {
      // remove a process P from S->list;
      ...
      wakeup(P);
    }
  }
  ```

  - When a process ust wait on a semaphore, it is added to the list of processes.
  - The `block()` and `wakeup()` operations are basic system calls (provided by the operating system).
  - If a semaphore is negative, its magnitude is the number of processes waiting on the semaphore.
  - Implementation of the waiting list:
    - can be a link field in each PCB–[Process Control Block](https://en.wikipedia.org/wiki/Process_control_block).
    - Each semaphore contains an integer value and a pointer to a list of PCBs.
    - can use any queueing strategy, like FIFO.
  - Busy waiting still exists: This implementation moves busy waiting from the entry section to the critical sections of the `wait()` and `signal()` opeartions.
  - Since these sections are short(if properly coded), the critical section is almost never occupied.

### 07 Implementation Obstacles
- Deadlocks
  - occurs when 2+ processes are waiting indefinitely for an event that caon be caused only by one of the waiting processes.
  - That is, processes that wait on each other will result in a deadlock.
  - Related Problem:
    - indefinite blocking / starvation: processes wait indefinitely within the semaphore
- Priority Inversion
  - occurs when a higher-priority process needs to read or modify kernel data that are currently being accessed by a lower-priority process.
  - Since the lower-priority process is lower-priority(duh), it will be preempted by a process with a higher priority.
  - Thus the waiting higher-priority process may wait for a long time because the lower-priority process keeps being preempted.
  - This problem occurs only in systems with 2+ priorities.
  - Solution: 'priority-inheritance protocol'
    - Al processes that are accessing resources needed by a higher-priority process inherit the higher priority until they are finished with the resources.
  - Example: [The Mars Pathfinder](http://www.knusbaum.com/mars/Authoritative_Account)


### 08 Classic Problems of Synchronization (the three tests to pass)
- These three synchronization problems are examples of a large class of concurrency-control problems.
- They are used for testing nearly every newly proposed synchronization scheme.

#### The Bounded-Buffer Problem
- commonly used to illustrate the power of synchronization primitives.
- the producer-consumer problem
- data structure:
```c
int n;
semaphore mutex = 1;
semaphore empty = n;
semaphore full = 0;
```

- structure of the producer process:
```c
do {
  // produce an item in next_produced

  wait(empty);
  wait(mutex);

  // add next_produced to the buffer

  signal(mutex);
  signal(full);
} while (true);
```

- structure of the consumer process:
```c
do {
  wait(full);
  wait(mutex);

  // remove an item from buffer to next_consumed

  signal(mutex);
  signal(empty);

  // consume the item next_consumed
} while (true);
```

#### The Readers-Writers Problem
- Frequently occurs in database management (database shared among several concurrent processes).
- Some processes only wants to read the data (readers), others want to update (writers).
- [The First Readers-Writers Problem] requires that no reader be kept waiting unless a writer has already obtained permission to use the shared object.
- [The Second Readers-Writers Problem] requires that the writer perform its write asap once it is ready.
- A solution to either problem may result in starvation.
  - The first problem, writers may starve.
  - The second problem, readers may starve.
- solution to the first readers-writers problem:
  ```c
  // data structure shared by the reader processes
  semaphore rw_mutex = 1;
  semaphore mutex = 1;
  int read_count = 0;

  // data structure of a writer process
  do {
    wait(rw_mutex);

    // perform writing

    signal(rw_mutex);
  } while (true);

  // data structure of a reader process
  do {
    wait(mutex);
    read_count++;
    if (read_count == 1)
      wait(rw_mutex);
    signal(mutex);

    // perform reading

    wait(mutex);
    read_count--;
    if (read_count == 0)
      signal(rw_mutex);
    signal(mutex);
  } while (true);
  ```

  - The `rw_mutex` semaphore is shared by both the reader and the writer.
  - The `mutex` semaphore is used to ensure mutual exclusion when the variable `read_count` is updated.
- Reader-writer locks are most useful in applications where
  - it is easy to indentify which processes only read shared data and which processes only write shared data.
  - readers > writers. Because reader-writer locks genrally require more overhead to establish than semaphores or mutex locks.So it has to be worth it.

#### The Dining Philosopher Problem
- a representation of the need to allocate several resources among several processes in a deadlock-free and starvation-free manner.
- Let each chopstick be a semaphore? Deadlocks could happen.
  ```c
  semaphore chopstick[5];

  do {
   wait(chopstick[i]);
   wait(chopstick[i+1 % 5]);

   // eat for awhile

   signal(chopstick[i]);
   signal(chopstick[i+1 % 5]);

   // think for awile
  } while (true);
  ```

- possible remedies:
  1. Allow at most (n-1) philosophers to be sitting simultaneously at the table.
  2. Asymmetric Solution: ann odd-numbered philosopher picks up first her left chopstick first, and an even-numbered philosopher picks up her right chopsticks first.
  3. Allow a philosopher to pick up her chopsticks only if both choopsticks are available (so she must pick them up in a critical section).
  --> example in the next chapter.

### 09 Monitors: The High-Level Synchronization Construct
- (for heedless programmers that have a tendency to mess up semaphores.)
- To reduce unintended errors, we introduce a high-level synchronization primitives–monitors with condition varialbles, that take care of semaphores for us.
- Underneath, the OS may implement monitors using semaphores and mutex locks.
- A function defined within a monitor can access only only those variables declared locally within the monitor and its formal parameters.
- The monitor construct ensures that only one process at a time is active within the monitor.
  --> so programmers does not need to code this synchronization construct explicitly.
- syntax:
  ```c
  monitor monitor_name
  {
    // shared variable declarations
    ...

      function P1(...) {
        ...
      }
      function P2(...) {
        ...
      }
      ...
      function Pn(...) {
        ...
      }

      initialization_code(...) {
        ...
      }
  }
  ```

- example:
```c
monitor sharedcounter {
  int counter;
  function add() { counter++; }
  function sub() { counter--; }
  init() { counter = 0; }
}
```

- Implicitly, a monitor
  - defines a mutex lock in the shared variables section.
  - defines mutexes around each function.

#### Condition Variables
- Provide additional mechanisms to monitors.
  - eg. signalling mechanism.
- A condition variable x in a monitor allows two operations on itself:
  - `x.wait()`: suspends the calling process until another process calls `x.signal()`
  - `x.signal()`: resumes exactly 1 suspended process. If none, then no effect.
    - This is unlike the semaphore's signalling operation, which preserves state in terms of the value of the semaphore.
    - eg. If a process Y calls `x.signal()` on a conditional variable x before process Z calls `x.wait()`, then Z will wait.
    - On the other hand, if Y calls `signal(mutex)` on a binary semaphore mutex(initialized to 0) before process Z calls `wait(mutex)`, then Z will not wait, because the semaphore remembers Y's signalling.
- What happens when a process P calls `x.signal()` to wake up a process Q waiting on x?
  - [Signal-and-Wait] Hoare semantics; P waits for either Q to leave the monitor or another condition variable.
  - [Signal-and-Continue] Mesa semantics; Q waits for either P to leave the monitor or another condition variable.

####  Monitor-based solution to the Dining Philosophers Problem
- key 1: pick up 2 chopsticks only if both are free.
  - That is, a philosopher moves to her eating state only if both neighbors are not in their eating states.
  - so we need to define a state for each philosopher.
- key 2: If one neighbor is eating, and the philosopher is hungry, ask that neighbor to `signal()` when she's done.
  - so the states of each philosopher are thinking, hungry, eating.
  - and we need condition variables to do the signalling.
- avoids deadlock

- implementation:
```c
monitor DiningPhilosophers
{
  enum { THINKING, HUNGRY, EATING } state[5];
  condition self[5];

  void pickup(int i) {
    state[i] = HUNGRY;
    test(i);
    if (state[i] != EATING)
      self[i].wait();  // if unable to eat, go to waitlist.
  }

  void putdown(int i) {
    state[i] = THINKING;
    test((i + 4) % 5);
    test((i + 1) % 5);
  }

  void test(int i) {
    if ((state[(i + 4) % 5] != EATING) &&
      (state[i] == HUNGRY) &&
      (state[(i + 1) % 5] != EATING)) {
      state[i] = EATING;
      self[i].signal();  // no effect during pickup(),
                        // but is important during putdown()
    }
  }

  initialization_code() {
    for (int i = 0; i < 5; i++)
      state[i] = THINKING;
  }
}
```

#### Resuming Processes within a Monitor
- If there are 2+ processes waiting to be signalled on a condition x, how to pick?
- The **Conditional Wait** construct: `x.wait(c)`
  - `c` is a **priority number**, stored with the name of the process.
  - When `x.signal()` is executed, the process with the smallest `c` is resumed next.
- ResourceAllocator, an example:
```c
monitor ResourceAllocator
{
  boolean busy;
  condition x;

  void acquire(int time) {
    if (busy)
      x.wait(time); // resumption priority is decided by
                    // submitted estimated 'time'.
      busy = true;
  }

  void release() {
    busy = false;
    x.signal();
  }

  initialization_code() {
    busy = false;
  }
}
```

#### Java Monitors
- Java provides  a monitor-like concurrency mechanism for thread synchronization.
- Every object in Java has associated with a single lock.
- When a method is declared `synchronized`, calling the method requires owning the lock for the object.
- If the lock is already in use by another thread, the requesting thread will be put into `entry set` for the object's lock.
- Java's `wait()` and `notify()` is similar to a monitor's `wait()` and `signal()`.
- More Java API support can be found in the `java.util.concurrent` package.
- example:
```java
public class SimpleClass {
  public synchronized void safeMethod() {
    // implementation of method
  }
}
```

### 10 [OS Synchronization Example] Windows
The Windows operating system is a **multithreaded kernel that provides support for real-time applications and multiple processors**.

#### Synchronization inside the Kernel
- On a single-processor system, Windows kernel temporarily masks interrupts for all interrupt handlers when accessing a global resource.
- On a multiprocessor system, Windows uses **spinlocks** to protect global resources (for short code segments).
- The kernel ensures that a thread will never be preempted while holding a spinlock (for efficiency).

#### Synchronization outside the Kernel
- Windows provides **dispatcher objects**.
- Dispatcher object types:
  - [mutex locks]
  - [semaphores]
  - [timers] used to notify 1+ thread(s) that a specified amount of time has expired.
  - [events] similar to condition variables.
- Dispatcher object states:
  - [signaled state] means the object is available.
  - [nonsignaled state] means the object is not available.
- **Critical Section Object** is a user-mode mutex that can often be acquired and released without kernel intervention.
  - It first uses a spinlock to wait.
  - If the spin is to long, the acquiring thread will allocate a kernel mutex and yield its CPU.
  - particularly efficient because the lernel mutex is allocated only when there is contention for the object.
  - In practice, there is very little conention.

### 11 [OS Synchronization Example] Linux
- `atomic_t`: the simplet synchronization technique within the Linux kernel.
  - All math operations using atomic integers are performed without interruption.
  - particularly efficient when an integer variable, like a counter, needs to be updated, because atomic operations do not require the overhead of locking mechanisms.
- `mutex_lock()` and `mutex_unlock()` are used to enter and leave a critical section.
  - A task is put to a sleeping state if the lock is unavailable.
- Spinlocks and semaphores (and the reader-writer versions of these two locks) are provided.
- On SMP(Symmetric MultiProcessing) machines, spinlock is the fundamental locking mechanism.
- On a single-processor system, the kernel disables kernel preemption.

#### Kernel Preemption
- A preemptive kernel allows a process to be preempted while it is running in kernel mode.
- A nonpreemptive kernel lets a process run until it exits kernel mode, blocks, or voluntarily yields control of the CPU.

### 12 [OS Synchronization Example] Solaris
- Critical Section Control Options: adaptive mutex locks, condition variables, semaphores, reader-writer locks, and turnstiles.

#### Adaptive Mutex
- It starts as a standard semaphore implemented as a spinlock.
- If the data are locked
  - if the lock-holding thread is running on another CPU --> the requesting thread spins.
  - if the lock-holding thread is not running --> the requesting thread blocks (going to sleep until awakened).
- Adaptive mutex is only used for short code segments (< a few hundred instructions).
  - If longer than that, condition variables and semaphores are used.
  - Or the spin-waiting method will be exceedingly inefficient.

#### Reader-Writer Lock
- best used for data that are accessed frequently in a read-only manner.
- efficient because multiple threads can read concurrently(whereas semaphores always serializes access to the data)
- relatively to implement, so used only on long sections of code.

#### Turnstile
- A turnstile is a queue structure containing threads blocked on a lock.
- used to order the list of threads waiting to acquire either an adaptive mutex or a reader-writer lock.
- The kernel maintains a list of free turnstiles.
- The kernel thread, not the synchronized object, is associated with the turnstile.
  - This is more efficient than having a turnstile for each object
  - because a thread can be blocked only on one object at a time
- **Priority Inheritance Protocol**
  - is used to prevent a priority inversion.
  - If a lower-priority thread currently holds a lock on which a higher-priority thread is blocked, the thread with the lower priority wil temporarily inherit the priority of the higher-priority thread.
- The kernel's locking mechanisms are implemented for user-level threads as well.
  - so the same type of locks are available inside and outside the kernel.
  - but user-level thread-locking mechanisms do not provide priority inheritance.

### 13 [User-Level Synchronization Example] Pthreads
- The synchronization methods discussed above are synchronization within the kernel.
- The Pthreads API is available for programmers at the user level and it is not part of any particular kernel.
- Mechanisms provided: mutex locks, condition variables, read-write locks.

#### Mutex Locks
- the fundamental synchronization technique used with Pthreads.
- `pthread_mutex_t`: data type for mutex locks.
- example:
  ```c
  #include <pthread.h>

  pthread_mutex_t mutex;

  // Create the mutex lock.
  // Initialized the mutex to its default attributes.
  pthread_mutex_init(&mutex, NULL);


  // protecting a critical section with mutex locks:

  pthread_mutex_lock(&mutex);

  // critical section

  pthread_mutex_unlock(&mutex);
  ```

- Many systems that implement Pthreads also provide semaphores, which belong to the POSIX SEM extension.
  - Two types of semaphores specified by POSIX: named and unnamed.
  - A **named semaphores** has an actual name in the file system and can be shared by multiple urelated processes.
  - An **unnamed semaphores** can be used only by threads of the same process.
- All mutex and semaphores functions return 0 with correct operation.
- Other extensions to the Pthreads API are available, including spinlocks.
  - But not all extensions are considered portable from one implementation to another.









14. Alternative Approaches
    - Transactional Memory
    - OpenMP
    - Functional Programming Languages

# [Operating System] The Dining Philosophers Problem

with semaphores and mutex + conditional variables
--------------------------------------------------------------------------
Last week's post introduced some concepts of the synchronization issue in operating systems. The main concept is to ensure that when multiple threads/tasks (whatever you call it) are running concurrently (that is, they all try to use the same resource at the same time without wanting to wait for others patiently).

Since it turned out that making these tasks run concurrently and with grace and harmony is actually not an easy kill, we put a lot of effort in finding/designing the best way to make all happy.

#### CONTENT
- What is the Dining Philosopher Problem?
- How does it relate to computer-related Problem?
  - multithreaded
- Some solutions to Consider
  - Resource Hierarchy Solution
  - Arbitrator Solution
  - Chandy/Misra Solution
- Implementation Options
  - semaphores
  - mutexes
  - monitors
- Implementation using Pthread API with C

### The Dining Philosopher Scenerio
Well, so the situation (I should like to call it a story but then it is not) is this:
1. Five (it does not have to be, but usually it is) philosophers are sitting at a rounded dining table.
2. There is ONE chopstick between each pair of adjacent philosophers.
3. Philosophers are either thinking or eating.
4. Whenever a philosopher wishes to eat, she will first have to pick up the chopsticks placed at her both side.
5. If she does not have two chopsticks (i.e. one or two of her neighbors already picked up the chopstick before her) she will have to wait until both chopsticks are available.
6. When a philosopher finishes eating, she puts down both chopsticks to their original places, and resumes thinking.
7. There is an infinite amount of food on their plate, so they only need to worry about the chopsticks.
8. Hopefully no philosophers should starve to death (i.e. wait over a certain amount of time before she gets to eat).

There are a few constraints:
1. Philosophers are either thinking or eating. They do not talk to each other.
2. Philosophers can only fetch chopsticks placed between them and their neighbors.
3. Philosophers cannot take their neighbors' chopsticks away while they are eating.

There are some variations to it, but for now let us look at this particular situation first.

### The Real Situation
As we probably have guessed, this is not just a problem of possible homicide of philosophers. The failures these philosophers may experience are the difficulties that arise in real computer programming when multiple programs need exclusive access to shared resources. It is about how to design a discipline of behavior–or more specifically, a concurrent algorithm–with the following two situation to avoid:

1. deadlocks
2. resource starvation (livelock)
3. Data Corruption

#### Deadlocks
> In an operating system, a deadlock occurs when a process or thread enters a waiting state because a requested system resource is held by another waiting process, which in turn is waiting for another resource held by another waiting process.
–[Wikipedia](https://en.wikipedia.org/wiki/Deadlock)

Basically, when a deadlock occurs, the program halts because no progress is available anymore. Think about what would happen if we instruct the philosophers as such (when they stops thinking and wishes to eat):
1. If both adjacent chopsticks are available, pick them up and eat (for sure).
2. If there is only one adjacent chopstick available at the time, pick up that chopstick while waiting for the other chopstick to become available.
3. If there are no chopsticks available, wait until any becomes available.

This is a simple and intuitive solution, except it will cause deadlocks in some unfortunate cases. When all five philosophers pick up their left chopstick at the same time, they will wait indifinitely because each is waiting for the others to put down their chopsticks.

#### Resource Starvation (Livelock)
> Resource starvation is a problem encountered in concurrent computing where a process is perpetually denied necessary resources to process its work.
–[Wikipedia](https://en.wikipedia.org/wiki/Starvation_(computer_science))

The term **starving** is quite self-explanatory. If we do not design our algorithm with care, the schedule may result in a situation where one philosopher is constantly bypassed for getting two chopsticks (therefore to eat).

**Finite Bypass** is an associated term, meaning that any process (concurrent part) of the algorithm is bypassd at most a finite number times before being allowed access to the shared resource. If an algorithm is starvation-free, it has finte bypass.

#### Data Corruption
> Data corruption refers to errors in computer data that occur during writing, reading, storage, transmission, or processing, which introduce unintended changes to the original data.
–[Wikipedia](https://en.wikipedia.org/wiki/Data_corruption)

In our philosopher dining situation, this is not a big concern. The closest thing to a data corruption here would be a scheduling that allows a philosopher to snatch her neighbors' chopsticks while they are eating. That leaves the unfortunate neighnors in an undefined state of 'half-full' or '70% full', which will cause unexpected results in the system.

### Some Solutions
There are some established, geniusly-designed solutions. We look at three of them: resource hierarchy solution, arbitrator solution, and Chandy/Misra solution.

#### Resource Hierarchy Solution
Dijkstra is the one who originally proposed this solution(who is also the one that formulated this problem). The idea is this:

1. Each resource (in this case, the chopsticks) is assigned a partial order.
2. All resources will be requested in order. That is, if a unit of work (a philosopher) needs chopstick #1 and #2, she needs to acquire chopstick #1 first and then #2.
3. The order in which the unit of work releases the resources does not matter. I.e. It does not matter if the philosopher puts down chopstick #1 or #2 first.

This design guarantees no deadlocks. The situation where all five philosophers pick up their chopsticks simultaneously would be impossible. If four philosophers have already picked up one chopstick each, only the highest-numbered chopstick will remain on the table, so the fifth philosopher cannot pickup any fork.

One major limitations to this solution is that it requires the list of resources be completely known in advance. If a unit of work finds out it needs a resource numbered 2 while holding resource #4 and #5, it would have to release the two resources first, acquire #2, and re-acquire #4 and #5. It could cause troubles in efficiency.

#### Arbitrator Solution
The idea of this solution is to introduce a third party that monitors the usage of resources. In this case, we would properly call it a waiter. The principles are as follows:

1. When unit of work wishes to access a resource, it asks the arbitrator for permission first. That is, when a philosopher wishes to eat, she first asks the waiter.
2. The arbitrator gives one permission at a time.
3. Only the unit that has the permission can access shared resources.
4. Releasing a resource does not need permissions.

There are two major setbacks for this solution. The first is the need to introduce a new central entity. The second is **reduced paralleism**: if a philosopher is eating and one of their neighbors requests the chopsticks, all other philosophers must wait until this request has been fulfilled even if their chopsticks are still on the table.

#### Chandy/Misra Solution
K.Mani Chandy and J.Misra introduced this solution in 1984. Principles:

1. The chopsticks have two state: clean and dirty. All chopsticks are initialized to dirty.
2. The chopsticks are allocated to the philosophers with the lower id in the pair at the first place (no chopsticks on the table).
3. When a philosopher wishes to eat, she must obtain the chopsticks from her neighbors. If she does not have the chopsticks, she sends a request message to the neighbor who has them.
4. When a philosopher with a chopstick receives a request message:
  - If the fork is clean, the philosopher keeps it.
  - If the fork is dirty, the philosopher cleans it and sends the fork over.
5. After eating, the philosopher's chopsticks become dirty. If another philosopher has previously requested one of the chopsticks, the philosopher cleans and sends it.

Some Strengths of this Algorithm:
- It allows for arbitrary agents to contend for an arbitrary number of resources.
- It is completely distributed and requires no central authority.
- It allows for a large degree of concurrency.
- It solves the starvation problem: the clean / dirty states act as a way of giving preference to the most 'starved' processes.

One setback of this design would be that it actually violates the rule of 'philosophers do not talk to each other', though.


### Implementation Options
So far we have only discussed concepts. Now we are really curious how are these concepts carried out in reality. Two options introduced here are `mutexes` and `semaphores`.

#### Mutex Locks
There are a couple of mechanisms that implement locks. Mutex (mutual exclusion) is the fundamental synchronization technique. The idea is simple: whenever a work unit accesses the **critical section**, it first needs a lock that guarantees no one else at this time is accessing the critical section. When the work unit exits the critical section, it returns the lock for other work units to access.

In the dining philosopher problem, we can implement an algorithm with mutexes that guarantee the philosophers not to be interrupted when they are changing their states (e.g. the process of picking up chopsticks).

Pthread API usage:
```c
#include <pthread.h>

// Create and initialized the mutex to its default attributes.
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);

// protecting a critical section with mutex locks:
pthread_mutex_lock(&mutex);

// critical section

// exiting the critical section
pthread_mutex_unlock(&mutex);
```

#### Semaphores
The usage of semaphores is very similar to using mutex locks. But the structure behind the two can be confusing sometimes if not paying attention. Instead of the binary operation in mutex locks (holding lock <-> releasing lock), semaphores function on the idea of 'number of resources'. Each semaphore is initialized to a number, indicating how many resources associated with that semaphore are available at that moment. When we use `sem_wait()` the number associated with that semaphore will decrease by one. When we use `sem_post()` the number associated with that semaphore will increase by one.

Yes, semaphores and mutex lock are interchangeable if there are only one resource assicoated with it (which will make it a binary semaphore).

Pthread API usage:
```c
#include <semaphore.h>


// Create and initialized the semaphore to its default attributes,
// and initialize the number of resources to 1.
sem_t mutex;
sem_init(&mutex, 0, 1);

// protecting a critical section with mutex locks
sem_wait(&mutex);

// critical section

// exiting the critical section
sem_post(&mutex);

```

### Implementation using Pthread API with C
I used

#### REFERENCES
- https://c-program-example.com/2012/02/c-program-to-solve-dining-philosophers-problem.html
- http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_cond_wait.html

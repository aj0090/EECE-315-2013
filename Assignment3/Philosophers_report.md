Dining Philosophers
===================

Overview and analysis of the dining philosophers problem and solution. This version is located in the *Basic* folder and can be made with `make` and run with `./philos`.

Problem Identification
----------------------

Dining Philosophers is a setup such that there are *n* philosophers seated at a round table with *n* forks (or some other utensil like chopsticks). The philosophers need to eat *m* meals. To eat a meal, a philosopher must have two forks. This is where the problem arises; how shall the philosophers coordinate themselves such that they are able to eat *m* meals with only *n* forks between all of them.

Process Planning
----------------

The problem asks us to solve the problem using pthreads only. This suggests that each philosopher be implemented as a thread fighting for some shared resource to complete a task. Using this mindset we can approach model selection.

Model Selection
---------------

The selected model in this case was **global resource ordering**. Each of the *n* forks was given a unique *fork\_id* related to the *philosopher\_id* of each philosopher such that to the left of each philosopher, *fork\_id = philosopher\_id*, and to the right of each philosopher, *fork\_id = (philosopher\_id + 1) % n*, where *%* is the modulo operator.

Then, each philosopher would check which fork was the **lower-numbered fork** and only attempt to pick that one up **first**. If the lower-numbered fork was unavailable the philosopher shall wait until it becomes available, and only once securing that fork, pick up the other fork.

The philosophers are implemented as an individual thread each which:

- for *n* meals:
    - thinks for a period of time
    - attempts to acquire both forks
    - eat a meal
    - put down both forks

The forks are implemented as mutexes.

Solution Evaluation
-------------------

The implemented solution works as expected. It always progresses and does not deadlock, therefore it finishes.

Assume the case with only two philosophers. Deadlock in this could occur if both philosophers each picked up a fork. However, this does not happen in our solution because both philosophers attempt to pick up the **lower-numbered fork first**. Both philosopher *0* and *1* would attempt to pick up fork *0*, however, only one of them will get it first and then subsequently pick up fork *1*, eat a meal, put down both forks, which is when the other philosopher will get a chance.

This is true for a base case of *n = 2* philosophers as shown, and will be true for any number of philosophers as the *0th* and *(n-1)th* philosopher will always attempt for fork *0* such as in the base case, therefore always preventing a deadlock.


Dining Philosophers 3D Animation
================================

I decided to learn a bit about IPC (inter-process communication) and do a 3D real-time animation of our dining philosphers solution as a bonus.
You are encouraged to either try the animation in the `Animated` folder by following the instruction in `README.md` in that folder, or to watch a video of it in action on [YouTube](http://www.youtube.com/watch?v=PgXC_XpOByA) or [Vimeo](https://vimeo.com/77948267).

Approach
-----
To do this, I thought the best solution would be to modify the C program to communicate with a Python program. The `philos.c` program from the original solution is kept and it sends out a message to the Python program everytime a philosopher picks up or puts down a fork.


Interprocess Communication
--------------------------
To do this, I used ZeroMQ sockets. This could have been done using just regular sockets but ZeroMQ is a nice library which makes this easier. The Python program initiates communication by sending a `Hello` and the C program responds whenever a thread (philosopher) has acquired a lock (fork). A three-character string containing the *fork_id*, *philosopher_id*, and state of the fork is sent. The Python program can then use this information to animate the forks.

3D Animation
------------
For the actual 3D animation, I used the Panda3D SDK. Panda3D is great SDK for rapid prototyping of games or animations such as this.

The program first parses the `constants.h` file to setup the chairs and forks, then starts the C program as a **subprocess**, starts receiving data and animating the forks. Tabs are kept on the number of forks each philosopher holds so when a philosopher is holding two forks, a bowl is placed in front of him simulating a meal being eaten.

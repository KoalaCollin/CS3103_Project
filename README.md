
# Parallel Matrix Operations

In this project, you will implement a parallel version of matrix operations, 
called `pmo`.

There are three specific objectives to this assignment:

* To familiarize yourself with the Linux pthreads package for threading.
* To learn how to parallelize a program.
* To learn how to program for performance.

## Background

To understand how to make progress on this project, you should first
understand the basics of thread creation, and perhaps locking and signaling
via mutex locks and condition variables. These are described in the following
book chapters:

- [Intro to Threads](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
- [Threads API](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf)
- [Locks](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf)
- [Using Locks](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf)
- [Condition Variables](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf)

Read these chapters carefully in order to prepare yourself for this project.

## Overview

To learn how to perform a series of matrix operations or how to implement a 
parallel version of matrix operations, please refer to the project pdf file.

Your parallel matrix operations (`pmo`) will use POSIX threads (pthreads) to 
accelerate the matrix operations. It will take one file as inputs, and 
output the result of given expression; the general usage from the command line 
will be as follows:

```Shell
[you@machine] ./pmo < input.txt > result.txt
```

## Considerations

Doing so effectively and with high performance will require you to address (at
least) the following issues:

- **How to parallelize the matrix operations.** Parallelizing the matrix 
    operations requires you to think about how to partition the matrix 
    operations, so that the computing resources can be fully used. Following 
    the mentioned first two ideas, you can design your own partition schemes. 
    The input matrix varies from different size, different dense/sparse degree. 
    Your schemes should take all those factors into consideration. 
- **How to split the expression for parallel calculation.** The matrix 
    operations in the expression can be calculated independently as mentioned 
    in the last ideas. The splitting scheme determines the amount of 
    computations of each thread. However, unbalanced splitting cannot make full 
    use of all threads. Your schemes should be a balance splitting scheme. 
    Besides, on Linux, the determination of the number of threads may refer to 
    some interfaces like get_nprocs() and get_nprocs_conf(); You are suggested 
    to read the man pages for more details.

## Grading

Your code should compile (and should be compiled) with the following flags:
`-Wall -pthread -O3`. The last one is important: it turns on the
optimizer! In fact, for fun, try timing your code with and without `-O3` and
marvel at the difference.

Your code will first be measured for correctness, ensuring that it zips input
files correctly.

If you pass the correctness tests, your code will be tested for performance;
higher performance will lead to better scores.

## Acknowledgements 

This project is taken (and slightly modified) from the [OSTEP projects](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/initial-utilities/README.md)
repository. 




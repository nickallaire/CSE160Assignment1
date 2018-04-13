# CSE 160: Introduction to Parallel Computing Assignment 1

Project Overview: The goal of this assignment is to create message-passing code to perform a parallel computation with any size ring of processors using Open MPI. The computation performed is: start ID sends the seed to its neighbor, when received add its MPI processor id to what it received, raises the sum to the power 1.2 and then sends the result to the next neighbor in the ring. When the start ID receives the result from its neighbor, it performs the same add + power computation and then sends the result to processor 0, who prints the result to stdout.

A complete description of the assignment can be found in PR1.pdf

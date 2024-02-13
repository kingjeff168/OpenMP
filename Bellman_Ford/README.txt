This program is to use OpenMP to calculate single-source shortest paths in a directed graph using a parallel version of the Bellman-Ford algorithm.
In this algorithm, the shortest-path estimates are updated each iteration by considering whether each edge can improve the estimates, with different threads handling different edges. 
Note that the number of iterations required may vary across different runs because of the asynchronous nature of the threads.

Run:
./bellmanford [filename] [number of threads] [assign the destination]

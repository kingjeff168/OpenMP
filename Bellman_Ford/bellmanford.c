/*
Last Date Modified: 1/9/2024

Description:

Use a program in C, including OpenMP, to calculate shortest paths in a directed graph using a parallel version of the Bellman-Ford algorithm.

*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

struct Edge 
{
    int from;
    int to;
    double weight;
};


void bellmanFord(struct Edge* edges, int numEdges, int numVertices, int source, double* distance, int* predecessor) 
{
    distance[source] = 0;

    // Parallelize the outer loop using OpenMP and iterates through all edges in the graph.
    #pragma omp parallel for
    for (int i = 0; i < numVertices - 1; ++i) 
    {
        // Inner loop remains sequential.
        for (int j = 0; j < numEdges; ++j) 
        {
            int u = edges[j].from;
            int v = edges[j].to;
            double weight = edges[j].weight;

            // Check if the distance to the source with the weight of the current edge is less than 
            // the current known distance to the destination.
            if (distance[u] + weight < distance[v]) 
            {
                #pragma omp critical
                {
                    // If the condition is true, update the distance and predecessor.
                    // Critical section is to prevent the race conditions.
                    if (distance[u] + weight < distance[v]) 
                    {
                        distance[v] = distance[u] + weight;
                        predecessor[v] = u;
                    }
                }
            }
        }
    }
        
}

void printShortestPaths(int numVertices, int destination, double* distance, int* predecessor) 
{
    // If the user specify the destination, it will print the destination and its corresponding shortest distance.
    // Otherwise, it will print all shortest paths of all destinations.
    if (destination != -1) 
    {
        printf("%d: %.5f; ", destination, distance[destination]);

        int current = destination;
        int path[numVertices];
        int index = 0;

        // Stores the current vertex and updates the current vertex to the array of predecessor.
        while (current != -1) 
        {
            path[index++] = current;
            current = predecessor[current];
        }

        // Start to print the path from 0.
        for (int j = index - 1; j >= 0; --j) 
        {
            printf("%d ", path[j]);
        }

        printf("\n");
    } 
    else 
    {
        // Print all shortest paths of all vertices.
        for (int i = 0; i < numVertices; ++i) 
        {
            printf("%d: %.5f; ", i, distance[i]);

            int current = i;
            int path[numVertices];
            int index = 0;

            // Stores the current vertex and updates the current vertex to the array of predecessor.
            while (current != -1) 
            {
                path[index++] = current;
                current = predecessor[current];
            }

            // Start to print the path from 0.
            for (int j = index - 1; j >= 0; --j) 
            {
                printf("%d ", path[j]);
            }

            printf("\n");
        }
    }
}

int main(int argc, char* argv[]) 
{

    FILE* inputFile = fopen(argv[1], "r");

    int numVertices, numEdges, numThreads;
    double startTime, endTime, elapsed_time;

    // Read the number of vertices and edges.
    fscanf(inputFile, "%d %d", &numVertices, &numEdges);

    struct Edge* edges = malloc(numEdges * sizeof(struct Edge));
    double* distance = malloc(numVertices * sizeof(double));
    int* predecessor = malloc(numVertices * sizeof(int));

    // Read the data of vertices and weight of edges.
    for (int i = 0; i < numEdges; ++i) 
    {
        fscanf(inputFile, "%d %d %lf", &edges[i].from, &edges[i].to, &edges[i].weight);
    }
    fclose(inputFile);


    // Set the number of threads the user want to use.
    numThreads = atoi(argv[2]);
    omp_set_num_threads(numThreads);

    // If there are the 3rd argument, then the destination will be assigned by it.
    int destination = -1;
    if (argc == 4) 
    {
        destination = atoi(argv[3]);
    }

    // Initialize the values of distance and predecessor.
    for (int i = 0; i < numVertices; ++i) 
    {
        distance[i] = INT_MAX;
        predecessor[i] = -1;
    }

    // Count the elapsed time of Bellman-Ford algorithm.
    startTime = omp_get_wtime();
    bellmanFord(edges, numEdges, numVertices, 0, distance, predecessor);
    endTime = omp_get_wtime();
    elapsed_time = endTime - startTime;
    
    

    // Print the shortest path and the elapsed time.
    printShortestPaths(numVertices, destination, distance, predecessor);
    printf("%.5f\n", elapsed_time);

    // Release the memory.
    free(edges);
    free(distance);
    free(predecessor);

    return 0;
}
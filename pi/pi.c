#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


float monteCarloEstimation(int randomPoints) 
{
    int pointsInsideCircle = 0;
    unsigned int seed = (unsigned int)time(NULL);


    // Do the parallel computing. The seed would local variable for the function.
    // After every thread finish the calculation, omp will add up their local pointsInsideCircle to global pointsInsideCircle.
    #pragma omp parallel for reduction(+:pointsInsideCircle) private(seed)
    for (int i = 0; i < randomPoints; ++i) 
    {
        seed = seed ^ omp_get_thread_num() ^ (unsigned int)time(NULL);

        float x = (float)rand_r(&seed) / RAND_MAX;
        float y = (float)rand_r(&seed) / RAND_MAX;

        float distance = x * x + y * y;


        if (distance < 1.0) 
        {
            pointsInsideCircle++;
        }
    }

    return 4.0 * ((float)pointsInsideCircle / randomPoints);
}

int main(int argc, char *argv[])
{
    int randomPoints, numThreads;
    float pi_estimate;
    double startTime, endTime, elapsed_time;

    // Read the total random points and the number of threads from command line.
    randomPoints = atoi(argv[1]);
    numThreads = atoi(argv[2]);

    // Set the number of threads.
    omp_set_num_threads(numThreads);

    // Start the time and calculate the value of pi.
    startTime = omp_get_wtime();
    pi_estimate = monteCarloEstimation(randomPoints);
    endTime = omp_get_wtime();
    elapsed_time = endTime - startTime;

    printf("%f, %lf\n", pi_estimate, elapsed_time);

    return 0;
}

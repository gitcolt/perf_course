#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "haversine.h"

#define LAT_MAX 90
#define LON_MAX 180

#define EARTH_RADIUS 6372.8

f64 rand_lat() {
    return (f64)rand() / ((f64)RAND_MAX/(LAT_MAX * 2)) - LAT_MAX;
}

f64 rand_lon() {
    return (f64)rand() / ((f64)RAND_MAX/(LON_MAX * 2)) - LON_MAX;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./main NUM_PAIRS\n");
        exit(EXIT_FAILURE);
    }

    int num_pairs = atoi(argv[1]);

    srand(time(NULL));

    f64 sum = 0;
    f64 sum_coef = 1.0 / (f64)num_pairs;

    printf("{\"pairs\": [\n");
    for (int i = 0; i < num_pairs; ++i) { 
        f64 x0 = rand_lon();
        f64 y0 = rand_lat();
        f64 x1 = rand_lon();
        f64 y1 = rand_lat();
        printf("    { \"x0\": %11f, \"y0\": %11f, \"x1\": %11f, \"y1\": %11f }%s\n",
                x0,
                y0,
                x1,
                y1,
                i == num_pairs - 1 ? "" : ",");

        f64 haversine_dist = ReferenceHaversine(x0, y0, x1, y1, EARTH_RADIUS);
        sum += sum_coef * haversine_dist;
    }
    printf("]}\n");

    printf("\n");
    printf("sum: %f\n", sum);
}

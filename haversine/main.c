#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "haversine.h"
#include "json_gen.h"

#define LAT_MAX 90
#define LON_MAX 180

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

    jg_start();
    jg_arr_start("pairs");
    for (int i = 0; i < num_pairs; ++i) { 
        jg_obj_start(NULL);
        jg_obj_key_val_float("x0", rand_lat());
        jg_obj_key_val_float("y0", rand_lon());
        jg_obj_key_val_float("x1", rand_lat());
        jg_obj_key_val_float("y1", rand_lon());
        jg_obj_end();
    }
    jg_arr_end();

    jg_end();

    jg_print();
    // const char *filename = "out.json";
    // jg_write_to_file(filename);

    jg_destroy();
}

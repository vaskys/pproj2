#include <iostream>
#include "config.h"
#include "ogl.h"
#include "shader.h"
#include <mpi.h>
#include <string>

int main(int argc, char** argv) {
    int ierr, num_procs, my_id;
    ierr = MPI_Init(NULL,NULL);

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    g_init(1280, 720, "fraktal");

    while (g_main_loop()) {
        g_clear_color(1, 0, 0);
        g_swap_buffer();

    }
    g_clear();

    ierr = MPI_Finalize();
    return 0;
}

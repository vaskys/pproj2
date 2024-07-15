#include <iostream>
#include <mpi.h>

using namespace std;

float mandelbrot(float cx,float cy,float iteracie) {
    float zx = 0.0f;
    float zy = 0.0f;
    float n = 0.0f;
    for(int i=0; i < iteracie; i++) {
        float znew_x = (zx * zx) - (zy * zy) + cx;
        float znew_y = (2.0 * zx * zy) + cy;
        zx = znew_x;
        zy = znew_y;

        if((zx * zy) + (zy *zy) > 100) {
            break;
        }
        n++;
    }
    return n/float(iteracie);
}

float farby_r(float t,float color_r) {
    float r = color_r * (1.0 - t) * t * t * t;
    return r;
}

float farby_g(float t,float color_g) {
    float g = color_g * (1.0 - t) * (1.0 - t) * t * t;
    return g;
}

float farby_b(float t,float color_b) {
    float b = color_b * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;
    return b;
}



int main() {
  int comm_size;

  MPI_Init(NULL,NULL);

  MPI_Comm parent;
  MPI_Comm_get_parent(&parent);

  int myid;
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  if (parent != MPI_COMM_NULL) {
    int parent_id;
    int screen_w;
    int screen_h;
    int iter;
    float zoom;
    float offset_x;
    float offset_y;
    int r_color;
    int g_color;
    int b_color;
    int pocet;

    MPI_Bcast(&parent_id, 1, MPI_INT,0, parent);
    MPI_Bcast(&screen_w, 1, MPI_INT,0, parent);
    MPI_Bcast(&screen_h, 1, MPI_INT,0, parent);
    MPI_Bcast(&iter, 1, MPI_INT,0, parent);
    MPI_Bcast(&zoom, 1, MPI_FLOAT,0, parent);
    MPI_Bcast(&offset_x, 1, MPI_FLOAT,0, parent);
    MPI_Bcast(&offset_y, 1, MPI_FLOAT,0, parent);
    MPI_Bcast(&r_color, 1, MPI_INT,0, parent);
    MPI_Bcast(&g_color, 1, MPI_INT,0, parent);
    MPI_Bcast(&b_color, 1, MPI_INT,0, parent);
    MPI_Bcast(&pocet, 1, MPI_INT,0, parent);


    const long size = screen_w * screen_h * 3;
    float *image_data_sub = new float[size/pocet];
    float *image_data; 

    int index =myid;

     for(int i=0; i < (size/pocet); i+=3) {
        int e = (i  + index  * (size/pocet)) / 3  ;
        int x = (e ) % screen_w;
        int y = ((e ) / screen_w ) % screen_h  ;
       
        float cy = y - (float)screen_h/2;
        float cx = x - (float)screen_w/2;
       
        cx = cx / zoom;
        cy = cy / zoom;
        cx = cx - offset_x;
        cy = cy - offset_y;
       
        float mb = mandelbrot(cx, cy, iter);
        float r = farby_r(mb, r_color);
        float g = farby_g(mb, g_color);
        float b = farby_b(mb, b_color);

        image_data_sub[i] = r;
        image_data_sub[i+1] = g;
        image_data_sub[i+2] = b;
     }

   // MPI_Send(image_data_sub, size/pocet, MPI_FLOAT, parent_id, 0, parent);
     MPI_Gather(image_data_sub, size/pocet, MPI_FLOAT, image_data, size, MPI_FLOAT, parent_id, parent);
  }
  MPI_Finalize();
  return 0;
}

#include "mb.h"
#include "framebuffer.h"
#include "ogl.h"

#include <omp.h>
#include <string>
#include <unistd.h>
#include <mpi.h>
#include <math.h>


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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MB::MB() {
    buffer = new FrameBuffer(g_get_screen_w(),g_get_screen_h());

    r = 9;
    g = 15;
    b = 8;

    iter = 100;
    offset_x = 0;
    offset_y = 0;
    zoom = 100;
    type = 0;
    n_omp_threads = 1;
    cas = 0.0f;
}

MB::~MB() {
}

void MB::clear() {
    buffer->clear();
    delete buffer;
}



void MB::update() {
    if(type == 0 )
        gpu();
    else if(type == 1) {
        omp();
    }
    else {
        mpi();
    }
}

void MB::gpu() {

    // auto start = high_resolution_clock::now();

    g_get_mb_shader()->use();
    g_get_mb_shader()->send_int_uniform("screen_w", g_get_screen_w())   ;
    g_get_mb_shader()->send_int_uniform("screen_h", g_get_screen_h());
    g_get_mb_shader()->send_float_uniform("zoom",this->get_zoom());
    g_get_mb_shader()->send_float_uniform("offset_x",this->get_offset_x());
    g_get_mb_shader()->send_float_uniform("offset_y",this->get_offset_y());
    g_get_mb_shader()->send_int_uniform("color_r", get_r());
    g_get_mb_shader()->send_int_uniform("color_g", get_g());
    g_get_mb_shader()->send_int_uniform("color_b", get_b());
    g_get_mb_shader()->send_int_uniform("iteracie", get_iter());

    g_draw_g_object();

    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // this->cas= duration.count() / 1000.0f;

}


#define CHUNK 100

void MB::omp() {
    float *image_data = new float[g_get_screen_h() * g_get_screen_w() * 3];
    auto start = high_resolution_clock::now();
    int pocet = this->n_omp_threads;

    #pragma omp parallel for schedule(dynamic,1) num_threads(pocet)
    for(int i=0; i < g_get_screen_h(); i++) {
        for( int j=0; j< g_get_screen_w(); j++) {
            double cy = i - (float)g_get_screen_h()/2;
            double cx = j - (float)g_get_screen_w()/2;
            cx = cx / this->zoom;
            cy = cy / this->zoom;
            cx = cx - this->offset_x;
            cy = cy - this->offset_y;

            float mb = mandelbrot(cx, cy, this->iter);
            float r = farby_r(mb, this->get_r());
            float g = farby_g(mb, this->get_g());
            float b = farby_b(mb, this->get_b());

            int index_jedna = (i*g_get_screen_w() + j) * 3;
            int index_dva = (i*g_get_screen_w() + j) * 3 + 1;
            int index_tri = (i*g_get_screen_w() + j) * 3 + 2;

            image_data[index_jedna] = r;
            image_data[index_dva] = g;
            image_data[index_tri] = b;
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    this->cas= duration.count() / 1000.0f;

    glBindTexture(GL_TEXTURE_2D, g_get_active_buffer()->texture);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,g_get_screen_w(),g_get_screen_h(),GL_RGB,GL_FLOAT,image_data);
    delete [] image_data;
}

void MB::mpi() {
    auto start = high_resolution_clock::now();
    const int size = g_get_screen_w() * g_get_screen_h() * 3;
    float *image_data = new float[size];

    MPI_Comm child;
    int spawnError[255];
    MPI_Comm_spawn("mpi_src/main.o", MPI_ARGV_NULL, this->get_omp_threads(), MPI_INFO_NULL, 0, MPI_COMM_SELF, &child, spawnError);

    int parent_id; 
    MPI_Comm_rank(MPI_COMM_WORLD, &parent_id);

    int w = g_get_screen_w();
    int h = g_get_screen_h();
    int iter = this->get_iter();
    float zoom = this->get_zoom();
    float off_x = this->get_offset_x();
    float off_y = this->get_offset_y();
    int c_r = this->get_r();
    int c_g = this->get_g();
    int c_b = this->get_b();
    int pocet = this->get_omp_threads();


    MPI_Bcast(&parent_id,1,MPI_INT,MPI_ROOT,child);
    MPI_Bcast(&w,1,MPI_INT,MPI_ROOT,child);
    MPI_Bcast(&h,1,MPI_INT,MPI_ROOT,child);
    MPI_Bcast(&iter,1,MPI_INT,MPI_ROOT,child);
    MPI_Bcast(&zoom,1,MPI_FLOAT,MPI_ROOT,child);
    MPI_Bcast(&off_x,1,MPI_FLOAT,MPI_ROOT,child);
    MPI_Bcast(&off_y,1,MPI_FLOAT,MPI_ROOT,child);
    MPI_Bcast(&c_r,1,MPI_INT,MPI_ROOT,child);
    MPI_Bcast(&c_g,1,MPI_INT,MPI_ROOT,child);
    MPI_Bcast(&c_b,1,MPI_INT,MPI_ROOT,child);
    MPI_Bcast(&pocet,1,MPI_INT,MPI_ROOT,child);


     float *image_data_sub;
     MPI_Gather(image_data_sub, size/pocet, MPI_FLOAT, image_data, size, MPI_FLOAT,MPI_ROOT,child);

    // for(int i=0; i < pocet; i++ ) {
    //     float *data = new float[size/pocet];
    //     MPI_Recv(data, size/pocet, MPI_FLOAT, MPI_ANY_SOURCE, 0, child, MPI_STATUS_IGNORE);
    //     for(int j=0; j< size/pocet; j++) {
    //         image_data[j+(i*size/pocet)] = data[j];
    //     }
    //
    //     delete [] data;
    // }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    this->cas= duration.count() / 1000.0f;
                
    glBindTexture(GL_TEXTURE_2D, g_get_active_buffer()->texture);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,g_get_screen_w(),g_get_screen_h(),GL_RGB,GL_FLOAT,image_data);
    delete [] image_data;
   // delete [] image_data_sub;
    //
}

void MB::reset() {
    r = 9;
    g = 15;
    b = 8;
    iter = 100;
    offset_x = 0;
    offset_y = 0;
    zoom = 100;
    n_omp_threads = 1;
}


void MB::set_type(int i) {
    this->type = i;
}

int MB::get_type() {
    return this->type;
}

void MB::set_r(int i) {
   this->r = i;
}

void MB::set_g(int i) {
   this->g = i;
}

void MB::set_b(int i) {
   this->b = i;
}

void MB::set_iter(int i) {
    this->iter = i;
}

void MB::set_zoom(float i) {
    this->zoom = i;
}

void MB::set_offset_x(float i) {
    this->offset_x = i;
}

void MB::set_offset_y(float i) {
    this->offset_y = i;
}


int MB::get_r() {
    return this->r;
}

int MB::get_g() {
    return this->g;
}

int MB::get_b() {
    return this->b;
}

int MB::get_iter() {
    return this->iter;
}

float MB::get_zoom() {
    return this->zoom;
}

float MB::get_offset_x() {
    return this->offset_x;
}

float MB::get_offset_y() {
    return this->offset_y;
}

FrameBuffer* MB::get_buffer() {
    return this->buffer;
}

int MB::get_omp_threads() {
    return n_omp_threads;
}

void MB::set_omp_threads(int i) {
    n_omp_threads = i;
}


void MB::omp_test() {
    omp_data.clear();
    for(int i=0; i<=7; i++) {
        set_omp_threads(pow(2,i));
        omp();
        omp_data.push_back(cas);
    }
}


void MB::gpu_test() {
    gpu_data.clear();
}


void MB::mpi_test() {
    mpi_data.clear();
    for(int i=1;i<4;i++) {
        set_omp_threads(i);
        mpi();
        mpi_data.push_back(cas);
    }
}

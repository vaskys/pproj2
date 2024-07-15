#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "config.h"

class FrameBuffer
{
    public:
        FrameBuffer(int w,int h);
        ~FrameBuffer();

       unsigned int id;
       unsigned int texture;
       unsigned int rbo;


       void clear();
};

#endif

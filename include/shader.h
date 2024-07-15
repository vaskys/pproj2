#ifndef SHADER_H
#define SHADER_H

#include "config.h"

class Shader
{
    public:
        Shader(string vertex_path,string fragment_path);
        ~Shader();

        void send_float_uniform(const string name,float v);
        void send_int_uniform(const string name,int v);

        void use();
        unsigned int get_id();
    protected:
        void compile(string vertex_code,string fragment_code);
        unsigned int id;
};


#endif

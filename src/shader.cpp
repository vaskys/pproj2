#include "shader.h"
#include <fstream>
#include <sstream>

Shader::Shader(string vertex_path,string fragment_path)
{
    string vertex_code;
    string fragment_code;

    fstream vertex_subor;
    fstream fragment_subor;

    vertex_subor.open(vertex_path);
    fragment_subor.open(fragment_path);
    
    stringstream vertex_stream;
    stringstream fragment_stream;

    vertex_stream<<vertex_subor.rdbuf();
    fragment_stream<<fragment_subor.rdbuf();

    vertex_code=vertex_stream.str();
    fragment_code=fragment_stream.str();

    vertex_subor.close();
    fragment_subor.close();
    
    compile(vertex_code,fragment_code);
}

Shader::~Shader()
{
    glDeleteShader(id);
}

void Shader::compile(string vertex_code,string fragment_code)
{
    unsigned int vertex;
    unsigned int fragment;

    int status;
    char log[512];

    const char* shader_code_vertex=vertex_code.c_str();
    const char* shader_code_fragemt=fragment_code.c_str();

    vertex=glCreateShader(GL_VERTEX_SHADER);    
    glShaderSource(vertex,1,&shader_code_vertex,NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex,GL_COMPILE_STATUS,&status);
    if(!status)
    {
        glGetShaderInfoLog(vertex,512,NULL,log);
        print("vertex error "+string(log));

    }

    fragment=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment,1,&shader_code_fragemt,NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment,GL_COMPILE_STATUS,&status);
    if(!status)
    {
        glGetShaderInfoLog(fragment,512,NULL,log);
        print("fragmet error "+string(log));
    }

    id=glCreateProgram();
    glAttachShader(id,vertex);
    glAttachShader(id,fragment);
    glLinkProgram(id);

    glGetProgramiv(id,GL_LINK_STATUS,&status);
    if(!status)
    {
        glGetProgramInfoLog(this->id,512,NULL,log);
        print("shader link error "+string(log));
        print(vertex_code);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(id);
}

unsigned int Shader::get_id()
{
    return id;
}

void Shader::send_float_uniform(const string name,float v) 
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), v); 
}

void Shader::send_int_uniform(const string name,int v) 
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), v); 
}


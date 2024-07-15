#version 330 core

out vec4 FragColor;
in vec2 UV;

uniform sampler2D fbo_buffer;

void main()
{
    vec3 color=texture(fbo_buffer,UV).rgb;
    FragColor=vec4(color,1.0);
}

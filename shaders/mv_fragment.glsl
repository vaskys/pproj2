#version 330 core
in vec4 gl_FragCoord;
 
out vec4 FragColor;

uniform int iteracie;
uniform int screen_w;
uniform int screen_h;
uniform float offset_x;
uniform float offset_y;
uniform float zoom;

uniform int color_r;
uniform int color_g;
uniform int color_b;

float n = 0.0f;
float threshold = 100.0;

float mandelbrot(vec2 c) {
    vec2 z = vec2(0.0,0.0);
    for(int i = 0; i < iteracie; i++ ) {
        vec2 znew;
        znew.x = (z.x * z.x ) - (z.y * z.y ) + c.x;
        znew.y = ( 2.0 * z.x * z.y) + c.y;
        z = znew;
        if((z.x * z.y) + (z.y * z.y) > threshold) {
            break;
        }
        n++;
    }
    return n/float(iteracie);
}

vec4 farby(float t) {									    
    float r = color_r * (1.0 - t) * t * t * t;
    float g = color_g * (1.0 - t) * (1.0 - t) * t * t; 
    float b = color_b * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;				
    return vec4(r, g, b, 1.0);
}
 
void main()
{
    vec2 coord = vec2(gl_FragCoord.xy);				
    coord = vec2(coord.x - screen_w/2, coord.y - screen_h/2);
    coord = coord/zoom;
    coord = vec2(coord.x - offset_x, coord.y-offset_y);

    float mb = mandelbrot(coord);	    
    FragColor = farby(mb);	
}

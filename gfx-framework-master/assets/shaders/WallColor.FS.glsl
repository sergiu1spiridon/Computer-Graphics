#version 330

// Uniform properties
uniform vec3 color;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(abs(vec3(0.46, 0.26, 0)), 1);
}

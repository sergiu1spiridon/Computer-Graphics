#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_color;
in vec2 frag_texture;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec3 out_texture;


void main()
{
    // TODO(student): Write pixel out color
    out_color = vec4(frag_color, 0.f);
	out_normal	= vec4(frag_normal, 1.f);
	out_texture = vec3(frag_texture, 1.f);
}

#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float time;
uniform vec3 object_color;

// Output value to fragment shader
out float noise;
out vec3 color;
out float alpha;

void main()
{
	// TODO: compute world space vectors
	vec3 world_position = vec3 (Model * vec4(v_position, 1));
	vec3 world_normal = normalize (vec3 (Model * vec4(v_normal, 0)));

	
	color = vec3(1);
	color *= object_color;
	alpha = 1;

	vec3 transformed = vec3(v_position);
	float freq = 7;
	float amp = 0.1;
	float angle = (time + v_position.y)*freq;
	
	if (time == 0 || time == 0.1) {
		gl_Position = Projection * View * Model * vec4(v_position, 1.0);
	} else {		
		transformed = v_position * atan(angle) * 0.8 + v_normal * cos(angle) * amp;
		noise = tan(angle); 
		
		gl_Position = Projection * View * Model * vec4(transformed, 1.0);
		//gl_Position = Projection * View * Model * vec4(v_position, 1.0);
	}

	if (time != 0) {
		float red = noise;
		float green = noise * 2;
		float blue = noise * 5;
		float alpha = noise * 10;
		
		color += vec3(red, green, blue);
	}
}
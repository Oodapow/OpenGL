#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 8) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

struct Light {
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

const float MAGNITUDE = 0.4;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
	vec4 pos = vec4((gl_in[0].gl_Position.x + gl_in[1].gl_Position.x + gl_in[2].gl_Position.x) / 3,
					(gl_in[0].gl_Position.y + gl_in[1].gl_Position.y + gl_in[2].gl_Position.y) / 3,
					(gl_in[0].gl_Position.z + gl_in[1].gl_Position.z + gl_in[2].gl_Position.z) / 3,
					1.0);
	gl_Position = pos;
    EmitVertex();
    gl_Position = pos + vec4(light.direction, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}  
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec4 plane;

void main()
{
	vec4 wp = model * vec4(aPos, 1.0);
	gl_ClipDistance[0] = dot(wp, plane);

    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyzw;
}  
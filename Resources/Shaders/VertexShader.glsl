#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec2 TexCoords;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;

void main()
{
	vec4 wp = model * vec4(aPos, 1.0);
	gl_ClipDistance[0] = dot(wp, plane);

    TexCoords = vec2(aPos.x, aPos.z);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  

    gl_Position = projection * view * wp;
}
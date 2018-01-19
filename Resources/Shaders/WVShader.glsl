#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 ClipSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	ClipSpace = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTextCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  

    gl_Position = ClipSpace;
}
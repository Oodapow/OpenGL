#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec4 ClipSpace;

uniform vec3 viewPos;
uniform Light light;
uniform sampler2D reflection;
uniform sampler2D refraction;

void main()
{
	vec2 ndc = (ClipSpace.xy/ClipSpace.w) / 2 + 0.5;
	FragColor = texture(refraction, vec2(ndc.x, -ndc.y));
} 
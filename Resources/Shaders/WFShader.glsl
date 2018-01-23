#version 330 core
out vec4 FragColor;

struct Light {
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec4 ClipSpace;

uniform vec3 viewPos;
uniform Light light;
uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

void main()
{
	float alpha = dot(normalize(viewPos - FragPos), vec3(0.0, 1.0, 0.0));
	vec2 ndc = (ClipSpace.xy/ClipSpace.w) / 2 + 0.5;

	vec4 refleC = texture(reflection, vec2(ndc.x, -ndc.y));
	vec4 refraC = texture(refraction, ndc);

	FragColor = mix(refleC, refraC, alpha);
	FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
} 
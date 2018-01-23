#version 330 core
out vec4 FragColor;

struct Light {
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec2 TexCoords;
in vec4 ClipSpace;

uniform vec3 viewPos;
uniform Light light;
uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform float windOffset;

const float waveStrength = 0.05;

void main()
{
	vec2 ndc = (ClipSpace.xy/ClipSpace.w) / 2 + 0.5;

	vec2 distort = (texture(dudvMap, vec2((TexCoords.x + windOffset) / 20, (TexCoords.y + windOffset) / 20)).rg * 2.0 - 1.0) * waveStrength
					+ (texture(dudvMap, vec2((TexCoords.x - windOffset) / 20, TexCoords.y / 20)).rg * 2.0 - 1.0) * waveStrength;

	vec2 refleTc = vec2(ndc.x + distort.x, -ndc.y - distort.y);
	vec2 refraTc = ndc + distort;

	refleTc.x = clamp(refleTc.x, 0.001, 0.999);
	refleTc.y = clamp(refleTc.y, -0.999, -0.001);
	refraTc = clamp(refraTc, 0.001, 0.999);

	vec4 refleC = texture(reflection, refleTc);
	vec4 refraC = texture(refraction, refraTc);

	vec4 normC = texture(normalMap, vec2((TexCoords.x + windOffset) / 20, (TexCoords.y + windOffset) / 20))
					+ texture(normalMap, vec2((TexCoords.x - windOffset) / 20, TexCoords.y / 20));
	vec3 norm = vec3(normC.r - 1.0, normC.b / 2, normC.g - 1.0);
	norm = normalize(norm);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(light.direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 20);
    vec3 specular = light.specular * spec;  

	float alpha = max(dot(normalize(viewPos - FragPos), norm), 0);
	alpha = pow(alpha, 0.25);

	FragColor = mix(refleC, refraC, alpha);
	FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specular, 0.0);
} 
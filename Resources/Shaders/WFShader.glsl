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
uniform sampler2D depthMap;

uniform float windOffset;

const float waveStrength = 0.05;

float near = 0.1; 
float far  = 1000.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	vec2 ndc = (ClipSpace.xy/ClipSpace.w) / 2 + 0.5;

	vec2 distort = (texture(dudvMap, vec2(TexCoords.x + windOffset, TexCoords.y + windOffset)).rg * 2.0 - 1.0) * waveStrength
					+ (texture(dudvMap, vec2(TexCoords.x - windOffset, TexCoords.y)).rg * 2.0 - 1.0) * waveStrength;

	vec2 refleTc = vec2(ndc.x + distort.x, -ndc.y - distort.y);
	vec2 refraTc = ndc + distort;

	refleTc.x = clamp(refleTc.x, 0.001, 0.999);
	refleTc.y = clamp(refleTc.y, -0.999, -0.001);
	refraTc = clamp(refraTc, 0.001, 0.999);

	vec4 refleC = texture(reflection, refleTc);
	vec4 refraC = texture(refraction, refraTc);

	if (length(vec3(refraC)) < 0.1) 
	{
		refraC = texture(refraction, ndc);
	}

	vec4 normC = texture(normalMap, vec2(TexCoords.x + windOffset, TexCoords.y + windOffset))
					+ texture(normalMap, vec2(TexCoords.x - windOffset, TexCoords.y));
	vec3 norm = vec3(normC.r - 1.0, normC.b / 2, normC.g - 1.0);
	norm = normalize(norm);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(light.direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 20);
    vec3 specular = light.specular * spec;  

	float alpha = max(dot(normalize(viewPos - FragPos), norm), 0);
	alpha = pow(alpha, 0.25);

	float d1 = LinearizeDepth(texture(depthMap, ndc).r);
	float d2 = LinearizeDepth(gl_FragCoord.z);
	float dw = d1 - d2;

	float intens = max(dot(vec3(0, -1, 0), light.direction), 0.2);

	float blueFactor = intens * (0.2 + smoothstep(0, 15, dw) * (0.75 - 0.2));
	refraC = mix(refraC, vec4(0.604, 0.867, 0.851, 1.0), blueFactor);

	FragColor = mix(refleC, refraC, alpha);
	FragColor = FragColor + vec4(specular, 0.0);
	FragColor.a = clamp(dw / 0.3, 0.0, 1.0);
} 
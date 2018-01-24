#version 330 core
out vec4 FragColor;

struct Light {
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 TexCoords;

uniform samplerCube skybox;
uniform Light light;


void main()
{    
	vec3 viewDir = normalize(TexCoords);
    float spec = pow(max(dot(viewDir, light.direction), 0.0), 70);
    vec3 specular = light.specular * spec;
    
	vec4 txt = texture(skybox, TexCoords);

	if ((txt.b - txt.r) > 0.16f && (txt.b - txt.g) > 0.16f )
	{
		if (specular.x > 0.99f && specular.y > 0.99f && specular.z > 0.99f)
		{
			FragColor = vec4(specular, 1.0f);
		}
		else
		{
			FragColor = txt * vec4(light.diffuse + light.ambient + specular * 0.5, 1.0);
		}
	}
	else
	{
		FragColor = txt * vec4(light.diffuse + light.ambient + specular * 0.2, 1.0);
	}
}
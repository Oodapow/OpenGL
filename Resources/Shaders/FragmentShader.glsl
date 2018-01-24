#version 330 core
out vec4 FragColor;

struct Light {
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Light light;
uniform sampler2D texture_d;
uniform sampler2D texture_s;
uniform sampler2D texture_n;
uniform sampler2D texture_d2;
uniform sampler2D texture_s2;
uniform sampler2D texture_n2;
uniform sampler2D texture_d3;
uniform sampler2D texture_s3;
uniform sampler2D texture_n3;
uniform sampler2D texture_d4;
uniform sampler2D texture_s4;
uniform sampler2D texture_n4;

void main()
{
	vec3 kd;
	vec3 ks;
	vec3 kn;

	if (FragPos.y >= 10) 
	{
		kd = texture(texture_d2, TexCoords).xyz;
		ks = texture(texture_s2, TexCoords).xyz;
		kn = texture(texture_n2, TexCoords).xyz;
	}
	else if (FragPos.y >= 8 && FragPos.y < 10)
	{
		float alpha = 1 - (FragPos.y - 8) / 2;
		kd = mix(texture(texture_d2, TexCoords), texture(texture_d3, TexCoords), alpha).xyz;
		ks = mix(texture(texture_s2, TexCoords), texture(texture_s3, TexCoords), alpha).xyz;
		kn = mix(texture(texture_n2, TexCoords), texture(texture_n3, TexCoords), alpha).xyz;
	}
	else if (FragPos.y >= 7 && FragPos.y < 8)
	{
		kd = texture(texture_d3, TexCoords).xyz;
		ks = texture(texture_s3, TexCoords).xyz;
		kn = texture(texture_n3, TexCoords).xyz;
	}
	else if (FragPos.y >= 5 && FragPos.y < 7)
	{
		float alpha = 1 - (FragPos.y - 5) / 2;
		kd = mix(texture(texture_d3, TexCoords), texture(texture_d, TexCoords), alpha).xyz;
		ks = mix(texture(texture_s3, TexCoords), texture(texture_s, TexCoords), alpha).xyz;
		kn = mix(texture(texture_n3, TexCoords), texture(texture_n, TexCoords), alpha).xyz;
	}
	else if (FragPos.y >= 2 && FragPos.y < 5)
	{
		kd = texture(texture_d, TexCoords).xyz;
		ks = texture(texture_s, TexCoords).xyz;
		kn = texture(texture_n, TexCoords).xyz;
	}
	else if (FragPos.y >= 0 && FragPos.y < 2)
	{
		float alpha = 1 - FragPos.y / 2;
		kd = mix(texture(texture_d, TexCoords), texture(texture_d4, TexCoords), alpha).xyz;
		ks = mix(texture(texture_s, TexCoords), texture(texture_s4, TexCoords), alpha).xyz;
		kn = mix(texture(texture_n, TexCoords), texture(texture_n4, TexCoords), alpha).xyz;
	}
	else
	{
		kd = texture(texture_d4, TexCoords).xyz;
		ks = texture(texture_s4, TexCoords).xyz;
		kn = texture(texture_n4, TexCoords).xyz;
	}


    vec3 ambient = light.ambient * kd;
  	
    // diffuse 
    vec3 norm = normalize(Normal);

	norm.x += (kn.r * 2 - 1);
	norm.y += kn.b;
	norm.z += (kn.g * 2 - 1);

    norm = normalize(norm);

    float diff = max(dot(norm, -light.direction), 0.0);
    vec3 diffuse = light.diffuse * diff * kd;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(light.direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * spec * length(ks);  
        
    vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);
} 
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

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 mdiffuse;

	if(FragPos.y <= 0) {
		float blue = - FragPos.y / 10.5;
		float green = 0.25;
		float red = 0.25;

		mdiffuse = vec3(red, green, blue);
	} else if(FragPos.y > 0) {
		float red = 0.25 + FragPos.y / 10.5;
		float blue = FragPos.y / 10.5;
		float green = 0.25 + FragPos.y / 10.5;

		mdiffuse = vec3(red, green, blue);
	}

    vec3 ambient = light.ambient * mdiffuse * 0.3;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, light.direction), 0.0);
    vec3 diffuse = light.diffuse * (diff * mdiffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(light.direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);
} 
#version 120

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

varying vec3 normal0;
varying vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

vec3 lightDir = vec3(0,0,1);

void main()
{
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * clamp(dot(-lightDirection, normal0),0.0,1.0);
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * clamp(dot(-vec3(0,0,1), normal0),0.0,1.0);

	//Ambient
	vec3 ambient = light.ambient * material.ambient;

        // Diffuse 
        vec3 norm = normalize(normal0);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = clamp(dot(norm, lightDir), 0.0,1.0);
        vec3 diffuse = light.diffuse * (diff * material.diffuse);

        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0,1.0), material.shininess);
        vec3 specular = light.specular * (spec * material.specular);  
 
	gl_FragColor = vec4(ambient+diffuse+specular, 1.0)* clamp(dot(-vec3(0,0,1), normal0),0.0,1.0);
	gl_FragColor.a = 1.0;

}

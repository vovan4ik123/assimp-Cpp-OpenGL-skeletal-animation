#version 430 core

in vec2 text_coords;
in vec3 frag_pos;
in vec3 normal;

struct Material
{
	sampler2D texture_diffuse1;

	sampler2D texture_specular1;

	float shininess;
	float transparency;
};

struct PointLight 
{
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular; 

    float constant;
    float linear;
    float quadratic;      
};

uniform Material material;
uniform PointLight point_light;
uniform vec3 view_pos;


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir, vec3 frag_pos)
{
	vec3 light_dir = normalize(light.position - frag_pos);
	//Diffuse
	float diff = max(dot(normal, light_dir), 0.0);
	//Specular 
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	//Attenuation
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, text_coords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, text_coords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, text_coords));

	
	ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    	
	return (ambient + diffuse + specular);
}

void main()
{
	vec3 view_dir = normalize(view_pos - frag_pos);

	vec4 calc_color = vec4( CalcPointLight(point_light, normal, view_dir, frag_pos), 1.0);
	calc_color.a *= material.transparency;
	
	gl_FragColor = calc_color;

}
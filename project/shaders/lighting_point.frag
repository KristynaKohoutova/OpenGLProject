#version 330 core

struct Material {

	vec3 ambient;
	sampler2D diffuseMap;
	vec3 specular;
	float shininess;
};

struct PointLight{

	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float exponent;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 frag_color;

uniform PointLight light;
uniform sampler2D texSampler1;
uniform Material material;
uniform vec3 viewPos;

void main()
{
	//ambient
	vec3 ambient = light.ambient * material.ambient * vec3(texture(material.diffuseMap, TexCoord));
	//diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float NDotL = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * vec3(texture(material.diffuseMap, TexCoord)) * NDotL;
	//specular - blinn phong
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0);
	vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess);

	float d = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * d + light.exponent * (d * d));

	diffuse *= attenuation;
	specular *= attenuation;


	frag_color = vec4(ambient + diffuse + specular, 0.8f);
}
